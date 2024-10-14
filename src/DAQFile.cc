/*!
 @file DAQFile.cc
 @brief Implementation of DAQFile class
 @version 0.1
 @date 2024-10-03
*/

#include "DAQFile.hh"

/*!
 @brief Construct a new DAQFile::DAQFile object
 */
DAQFile::DAQFile() : init_(false) {}

/*!
 @brief Construct a new DAQFile object
 @param filename The name of the file to open
 */
DAQFile::DAQFile(const std::string &filename)
    : filename_(filename), init_(false), in_(filename, std::ios::binary)
{
    std::cout << "File " << filename << " opened" << std::endl;

    if (!in_.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    (*this).Initialise();
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

DAQFile &DAQFile::ReadWord()
{
    if (!in_.good())
    {
        std::cerr << "!! ERROR: Unable to read file " << filename_ << std::endl;
    }

    DAQFile &file = *this;
    char w[4];

    if (!init_)
    {
        in_.read(w, 4); // Reading FILE HEADER
        if (w[0] == 'D' && w[1] == 'R' && w[2] == 'S')
        {
            if (w[3] == '8')
            {
                std::cout << "WaveDREAM Board" << std::endl;
                type_ = WDB;
            }
            else
            {
                std::cout << "DRS Evaluation Board" << std::endl;
                type_ = DRS;
            }
            in_.read(w, 4); // Reading TIME HEADER
        }
        else if (strcmp(w, "TIME") == 0) // For LAB-DRS first line is TIME HEADER
        {
            std::cout << "LAB-DRS Evaluation Board" << std::endl;
            type_ = LAB;
        }
        else
        {
            std::cerr << "!! ERROR: Invalid file header" << std::endl
                      << "Expected \"DRS\", found " << w << std::endl;
            std::cerr << "Initialisation failed" << std::endl;
            return file;
        }

        if (strcmp(w, "TIME") != 0)
        {
            std::cerr << "!! ERROR: Invalid time header" << std::endl
                      << "Expected \"TIME\", found " << w << std::endl;
            std::cerr << "Initialisation failed" << std::endl;
            return file;
        }
    }

    return file;
}

DAQEvent *DAQFile::CreateEvent(BoardType type)
{
    switch (type)
    {
    case DRS:
        return new DRSEvent();
    case WDB:
        return new WDBEvent();
    case LAB:
        return new LABEvent();
    default:
        return nullptr;
    }
}

/*!
 @brief Initialise the file reading the **TIME** block.

 @return DAQFile&
 */
DAQFile &DAQFile::Initialise()
{
    DAQFile &file = *this;
    char word[4];

    if (in_.is_open() == false)
    {
        std::cerr << "!! ERROR: File not open " << std::endl
                  << "Use DAQFile::Open()" << std::endl;
        return file;
    }

    if (init_ == true)
    {
        std::cout << "File already initialised" << std::endl;
        return file;
    }

    file.ReadWord();
    init_ = true;

    return file;
}