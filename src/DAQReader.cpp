#include <iostream>

#include "DAQReader.h"
#include "DAQException.h"
#include "DAQCommon.h"

DAQFile::DAQReader::DAQReader(const std::string &filename)
    : _in(filename, std::ios::in | std::ios::binary),
      _fileName(filename),
      _init(false),
      _board(0),
      _channel(0)
{
    try
    {
        if (_in.fail())
            throw std::ios_base::failure("ERROR: Could not open file \"" + filename + "\"");
        std::cout << "File \"" << filename << "\" opened successfully\n";
        _init = Initialize();
        if (!_init)
            throw std::runtime_error("ERROR: Could not initialize file \"" + filename + "\"");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::exit(EXIT_FAILURE);
    }
}

bool DAQFile::DAQReader::Initialize()
{
    std::cout << "Initializing file\n";
    try
    {
        _in.seekg(0);
        Read_F_HEADER();
        Read_T_HEADER();

        unsigned int iBoard = 0;
        unsigned int jChannel = 0;
        while (Read_B_HEADER())
        {
            while (Read_C_HEADER())
            {
                //_reader->Read_T_ARRAY();
                _in.seekg(4096, std::ios::cur);
                jChannel++;
            }
            iBoard++;
            jChannel = 0;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return _in.good();
}

/*!
 @brief Read File Header
 @details Read the file header and determine the board type. If the board type is not recognized, an error message is printed and the function returns false. Otherwise, the function returns true.

 @return bool True if the file stream is good, otherwise false
 */
bool DAQFile::DAQReader::Read_F_HEADER()
{
    char line[4];
    _in.read(line, sizeof(line));
    if (std::string(line, 4) == "DRS8")
    {
        _boardType = BoardType_t::WDB;
        std::cout << "Model: WaveDREAM Board\n";
    }
    else if (std::string(line, 4).substr(0, 3) == "DRS")
    {
        _boardType = BoardType_t::DRS;
        std::cout << "Model: " << std::string(line, 4) << " Board\n";
    }
    else if (std::string(line, 4) == "TIME")
    {
        _boardType = BoardType_t::LAB;
        std::cout << "Model: LAB-DRS Board\n";
    }
    else
    {
        std::cerr << "ERROR: Unknown board type\n";
        return false;
    }
    return _in.good();
}

/*!
 @brief Read Time Header
 @details Read the time header.

 @return bool True if the file stream is good, otherwise false
 */
bool DAQFile::DAQReader::Read_T_HEADER()
{
    switch (_boardType)
    {
    case BoardType_t::WDB:
    case BoardType_t::DRS:
        char line[4];
        _in.read(line, sizeof(line));
        if (std::string(line, 4) != "TIME")
            throw DAQException(std::string(line, 4), BlockType_t::T_HEADER);
        break;
    case BoardType_t::LAB:
        break;
    }

    return _in.good();
}

bool DAQFile::DAQReader::Read_B_HEADER()
{
    char board[2];
    uint16_t id;
    _in.read(board, sizeof(board));
    _in.read(reinterpret_cast<char *>(&id), sizeof(id));
    if (!_init && std::string(board, 2) == "B#")
    {
        _board++;
        if (_board == 1)
            std::cout << "Board list: " << std::string(board, 2) << id;
        else
            std::cout << "            " << std::string(board, 2) << id;
    }
    else if (!_init && std::string(board, 2) == "EH")
    {
        _in.seekg(-4, std::ios::cur);
        return false;
    }
    else
        throw DAQException(std::string(board, 2), BlockType_t::B_HEADER);

    return _in.good();
}

bool DAQFile::DAQReader::Read_C_HEADER()
{
    char line[4];
    _in.read(line, sizeof(line));

    if (line[0] == 'C')
    {
        auto id = std::stoi(std::string(line, 4).substr(1, 3));
        _channel++;
        return _in.good();
    }
    else if (std::string(line, 4) == "EHDR" || std::string(line, 4).substr(0, 2) == "B#")
    {
        _in.seekg(-4, std::ios::cur);
        if (!_init)
            std::cout << " (" << _channel << " channels)\n";
        return false;
    }
    else
    {
        throw DAQException(std::string(line, 4), BlockType_t::C_HEADER);
        return false;
    }
    return _in.good();
}

bool DAQFile::DAQReader::Read_T_ARRAY()
{
    return _in.good();
}