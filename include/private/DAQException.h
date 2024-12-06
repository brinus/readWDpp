#ifndef DAQEXCEPTION_H
#define DAQEXCEPTION_H

#include <exception>
#include <string>

#include "DAQCommon.h"

class DAQException : public std::exception
{
public:
    DAQException(const std::string & word, const BlockType_t & type);
    ~DAQException() throw() {}

    const char *what() const throw();

private:
    std::string _msg; ///< The message to be thrown.
};

#endif
