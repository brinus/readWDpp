

#include "DAQFile.h"
#include "DAQCommon.h"
#include "DAQReader.h"

/*!
 @brief Construct a new DAQFile::DAQFile object
 @details Constructor for DAQFile class, initializes file stream and calls Initialize() function. If the file stream fails to open, an exception is thrown. If the Initialize() function fails, an exception is thrown.

 @param filename Name of the file to open
 */
DAQFile::DAQFile(const std::string &filename) : _reader(std::make_unique<DAQReader>(this)),
                                                _in(filename, std::ios::in | std::ios::binary),
                                                _fileName(filename),
                                                _init(false)
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

/*!
 @brief Destroy the DAQFile::DAQFile object
 @details Destructor for DAQFile class, closes the file stream if it is open.
 */
DAQFile::~DAQFile()
{
    if (_in.is_open())
        _in.close();
}

/*!
 @brief Initialize the file
 @details Initialize the file by reading the file header and setting the file stream to the beginning of the file. The function returns true if the file stream is good, otherwise it returns false.

 @return bool True if the file stream is good, otherwise false
 */
bool DAQFile::Initialize()
{
    std::cout << "Initializing file\n";

    try
    {
        _in.seekg(0);
        _reader->Read_F_HEADER();
        _reader->Read_T_HEADER();

        unsigned int iBoard = 0;
        unsigned int jChannel = 0;
        while (_reader->Read_B_HEADER())
        {
            while (_reader->Read_C_HEADER())
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