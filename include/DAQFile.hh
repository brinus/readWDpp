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

#include "DAQCommon.hh"
#include "DAQEvent.hh"

class DAQFile
{
public:
    DAQFile(const std::string &);
    ~DAQFile();

    DAQFile &Open(const std::string &);
    DAQFile &Close();
    DAQFile &Reset();

private:
    enum BoardType
    {
        DRS, ///< DRS board.
        WDB, ///< WDB board.
        LAB  ///< LAB board.
    }; ///< Enum for board type.

    std::ifstream in_;      ///< Input file stream.
    std::string filename_;  ///< File name.
    unsigned int firstPos_; ///< First event position of the file.
    bool init_;             ///< Flag to check if the file has been initialised.

    BoardType type_; ///< Board type.
    MAP times_;      ///< Array of times.

    DAQFile();

    DAQFile &ReadWord();

    DAQFile &Initialise();
    DAQEvent *CreateEvent(BoardType);
};

#endif