#include "DAQReader.h"
#include "DAQException.h"

/*!
 @brief Construct a new DAQFile::DAQReader object
 @details Constructor for DAQReader class, sets the parent pointer to the DAQFile object.

 @param file Pointer to the DAQFile object
 @see DAQFile
 @see DAQReader
 */
DAQFile::DAQReader::DAQReader(DAQFile *const file) : _parent(file)
{
}

/*!
 @brief Read File Header
 @details Read the file header and determine the board type. If the board type is not recognized, an error message is printed and the function returns false. Otherwise, the function returns true.

 @return bool True if the file stream is good, otherwise false
 */
bool DAQFile::DAQReader::Read_F_HEADER()
{
    char line[4];
    _parent->_in.read(line, sizeof(line));
    if (std::string(line, 4) == "DRS8")
    {
        _parent->_boardType = BoardType_t::WDB;
        std::cout << "Model: WaveDREAM Board\n";
    }
    else if (std::string(line, 4).substr(0, 3) == "DRS")
    {
        _parent->_boardType = BoardType_t::DRS;
        std::cout << "Model: " << std::string(line, 4) << " Board\n";
    }
    else if (std::string(line, 4) == "TIME")
    {
        _parent->_boardType = BoardType_t::LAB;
        std::cout << "Model: LAB-DRS Board\n";
    }
    else
    {
        std::cerr << "ERROR: Unknown board type\n";
        return false;
    }
    return _parent->_in.good();
}

/*!
 @brief Read Time Header
 @details Read the time header.

 @return bool True if the file stream is good, otherwise false
 */
bool DAQFile::DAQReader::Read_T_HEADER()
{
    switch(_parent->_boardType)
    {
        case BoardType_t::WDB:
        case BoardType_t::DRS:
            char line[4];
            _parent->_in.read(line, sizeof(line));
            if (std::string(line, 4) != "TIME")
                throw DAQException(std::string(line, 4), BlockType_t::T_HEADER);
            break;
        case BoardType_t::LAB:
            break;
    }

    return _parent->_in.good();
}

bool DAQFile::DAQReader::Read_B_HEADER()
{
    char board[2];
    uint16_t id;
    _parent->_in.read(board, sizeof(board));
    _parent->_in.read(reinterpret_cast<char *>(&id), sizeof(id));
    if (!_parent->_init && std::string(board, 2) == "B#")
    {
        _board++;
        if (_board == 1)
            std::cout << "Board list: " << std::string(board, 2) << id;
        else 
            std::cout << "            " << std::string(board, 2) << id;
    }
    else if (!_parent->_init && std::string(board, 2) == "EH")
    {
        _parent->_in.seekg(-4, std::ios::cur);
        return false;
    }
    else
        throw DAQException(std::string(board, 2), BlockType_t::B_HEADER);

    return _parent->_in.good();
}

bool DAQFile::DAQReader::Read_C_HEADER()
{
    char line[4];
    _parent->_in.read(line, sizeof(line));

    if (line[0] == 'C')
    {
        auto id = std::stoi(std::string(line, 4).substr(1, 3));
        _channel++;
        return _parent->_in.good();
    }
    else if (std::string(line, 4) == "EHDR" || std::string(line, 4).substr(0,2) == "B#")
    {
        _parent->_in.seekg(-4, std::ios::cur);
        if (!_parent->_init)
            std::cout << " (" << _channel << " channels)\n";
        return false;
    }
    else 
    {
        throw DAQException(std::string(line, 4), BlockType_t::C_HEADER);
        return false;
    }
    return _parent->_in.good();
}