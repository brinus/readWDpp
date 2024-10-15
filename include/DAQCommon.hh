/*!
 @file DAQCommon.hh
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Common definitions for DAQFile and related classes
 @version 0.1
 @date 2024-10-03

 @copyright Copyright (c) 2024
*/

#ifndef DAQCOMMON_H
#define DAQCOMMON_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <array>
#include <map>
#include <fstream>
#include <stdexcept>

#define SAMPLES_PER_WAVEFORM 1024 ///< The number of samples made by the waveforms, both DRS and WDB.

using MAP = std::map<int, std::map<int, std::vector<float>>>; ///< Alias for data structure.

enum FileTag
{
    F_HEADER,
    T_HEADER,
    B_HEADER,
    C_HEADER,
    E_HEADER,
}; ///< Enum for tags.

enum BoardType
{
    DRS, ///< DRS board.
    WDB, ///< WDB board.
    LAB  ///< LAB board.
}; ///< Enum for board type.

/*!
 @brief Informations about the event contained in the file.

 Everytime an @ref EventHeader is read, the infos about the event are read in order just as specified in the
 DRS Evaluation Board manual.
 */
struct EventHeader
{
    char tag[4];                ///< The tag of the event header.
    unsigned int serialNumber;  ///< The serial number of the event: starting from 1 for DRS and 0 for WDB.
    unsigned short year;        ///< The year.
    unsigned short month;       ///< The month.
    unsigned short day;         ///< The day.
    unsigned short hour;        ///< The hour.
    unsigned short min;         ///< The minute.
    unsigned short sec;         ///< The second.
    unsigned short ms;          ///< The millisecond.
    unsigned short rangeCenter; ///< The rangeCenter (in Volts).
};

#endif // COMMONDEFINITIONS_H