/*!
 @file DAQFile.cc
 @brief Implementation of DAQFile class
 @version 0.1
 @date 2024-10-03
*/

#include "DAQFile.hh"
#include "DAQException.hh"

/*!
 @brief Construct a new DAQFile object
 */
DAQFile::DAQFile() {}

/*!
 @brief Construct a new DAQFile object
 @param filename The name of the file to open
 */
DAQFile::DAQFile(const std::string &filename)
    : filename_(filename),
      init_(false),
      in_(filename, std::ios::binary)
{
    try
    {
        if (!in_.is_open())
            throw std::runtime_error("Failed to open file: " + filename);
        std::cout << "File opened: " << filename << std::endl;
        init_ = Initialise();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }
}

/*!
 @brief Destroy the DAQFile::DAQFile object
 */
DAQFile::~DAQFile()
{
    if (in_.is_open())
    {
        in_.close();
    }
}

/*!
 @brief Initialise the file
 @return true if the file has been initialised, false otherwise
 */
bool DAQFile::Initialise()
{
    std::cout << "Initialising file..." << std::endl;
    std::string word(4, '\0');
    DAQFile &file = *this;

    try // Read the FILE HEADER
    {
        in_.read(reinterpret_cast<char *>(&word[0]), 4);
        if (word == "DRS8")
        {
            type_ = WDB;
            std::cout << "WaveDREAM Board found" << std::endl;
            in_.read(reinterpret_cast<char *>(&word[0]), 4);
        }
        else if (word.substr(0, 3) == "DRS")
        {
            type_ = DRS;
            std::cout << "DRS Evaluation Board found" << std::endl;
            in_.read(reinterpret_cast<char *>(&word[0]), 4);
        }
        else if (word == "TIME")
        {
            type_ = LAB;
            std::cout << "LAB-DRS Evaluation Board found" << std::endl;
        }
        else
        {
            throw DAQException(word, FileTag::F_HEADER);
        }
    }
    catch (const DAQException &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    try // Read the TIME HEADER
    {
        if (word != "TIME")
        {
            throw DAQException(word, FileTag::T_HEADER);
        }
    }
    catch(const DAQException &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    int i = 0;    
    for (auto line : file)
    {
        if (i == 100) break;
        std::cout << i << ": " << line << std::endl;
        ++i;
    }

    return true;
}