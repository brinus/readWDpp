/*!
 @file DAQFile.cc
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Definition of methods for class DAQFile.
 @version 0.1
 @date 2024-10-03

 @copyright Copyright (c) 2024

 */

#include "DAQFile.hh"

template <typename T>
DAQFile &DAQFile::Read(T t)
{
    if (!in_.is_good())
    {
        std::cerr << "!! ERROR: Unable to read file " << filename_ << std::endl;
    }
    else if (sizeof(t) == sizeof(char) * 4) // Reading a char[4]
    {
        if (!init)
        {
            in_.read(word, 4); // Reading FILE HEADER
            if (t[0] == 'D' && t[1] == 'R' && t[2] == 'S')
            {
                if (t[3] == '8')
                {
                    std::cout << "WaveDREAM Board" << std::endl;
                    type_ = WDB;
                }
                else
                {
                    std::cout << "DRS Evaluation Board" << std::endl;
                    type_ = DRS;
                }
                in_.read(t, 4); // Reading TIME HEADER
            }
            else if (strcmp(t, "TIME") == 0) // For LAB-DRS first line is TIME HEADER
            {
                std::cout << "LAB-DRS Evaluation Board" << std::endl;
                type_ = LAB;
            }
            else
            {
                std::cerr << "!! ERROR: Invalid file header" << std::endl
                          << "Expected \"DRS\", found " << t << std::endl;
                std::cerr << "Initialisation failed" << std::endl;
                return file;
            }

            if (strcmp(t, "TIME") != 0)
            {
                std::cerr << "!! ERROR: Invalid time header" << std::endl
                          << "Expected \"TIME\", found " << t << std::endl;
                std::cerr << "Initialisation failed" << std::endl;
                return file;
            }
        }
        else
        {

        }
    }
}

DAQEvent *DAQFile::CreateEvent(EventType type)
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
    unsigned short b = 0, c = 0;


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

    file.Read(word);
    init_ = true;

    return;
}