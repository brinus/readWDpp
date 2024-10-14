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

#endif // COMMONDEFINITIONS_H