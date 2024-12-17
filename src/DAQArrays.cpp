#include <iostream>

#include "DAQCommon.h"

TimeArray_t::TimeArray_t(const float &time)
{
    for (auto &t : _timeArray)
        t = time;
}

void TimeArray_t::Calibrate(const unsigned int &tCell)
{
    std::cout << "[INFO] Skipping calibration for now" << std::endl;
}

VoltArray_t::VoltArray_t(const float &voltage)
{
    for (auto &v : _voltArray)
        v = voltage;
}

void VoltArray_t::Calibrate(const unsigned int &range)
{
    std::cout << "[INFO] Skipping calibration for now" << std::endl;
}