/*!
 @file DAQFile.hh
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Definition of class DAQFile
 @version 0.1
 @date 2024-10-03

 @copyright Copyright (c) 2024

 */

#ifndef DAQFILE_H
#define DAQFILE_H

#include <iostream>
#include <map>
#include <string>
#include <array>
#include <fstream>
#include "DAQEvent.hh"

class DAQFile
{
    using MAP = std::map<int, std::map<int, std::array<float, 1024>>>; ///< Alias for data structure.

public:
    DAQFile();
    DAQFile(const std::string &);
    ~DAQFile();

    DAQFile &Close();
    DAQFile &Open(const std::string &);
    DAQFile &Reset();

private:
    enum BoardType
    {
        DRS, ///< DRS board.
        WDB, ///< WDB board.
        LAB  ///< LAB board.
    }; ///< Enum for board type.

    std::ifstream in_;        ///< Input file stream.
    std::string filename_;    ///< File name.
    unsigned int firstPos_;   ///< First event position of the file.
    bool init_;               ///< Flag to check if the file has been initialised.
    BoardType type_;          ///< Board type.
    MAP times_; ///< Array of times.

    template<typename T>
    DAQFile &Read(T t);

    DAQFile &Initialise();
    DAQEvent *CreateEvent(BoardType type);
};

#endif