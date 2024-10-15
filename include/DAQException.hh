/*!
 @file DAQException.hh
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief 
 @version 0.1
 @date 2024-10-15
 
 @copyright Copyright (c) 2024
 
 */

#ifndef DAQEXCEPTION_H
#define DAQEXCEPTION_H

#include "DAQCommon.hh"

class DAQException : public std::exception
{
public:
    DAQException(const std::string &, FileTag);
    ~DAQException() throw() {}

    const char *what() const throw();

private:
    std::string msg_; ///< The message to be thrown.

    DAQException();
};

#endif