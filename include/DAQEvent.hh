/*!
 @file DAQEvent.hh
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Definition of class DAQEvent
 @version 0.1
 @date 2024-10-03
 
 @copyright Copyright (c) 2024
 
 */

#ifndef DAQEVENT_H
#define DAQEVENT_H

#include "DAQCommon.hh"

class DAQEvent {};

class DRSEvent : public DAQEvent {};
class WDBEvent : public DAQEvent {};
class LABEvent : public DAQEvent {};

#endif