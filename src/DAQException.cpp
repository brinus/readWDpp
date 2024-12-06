/*!
 @file DAQException.cc
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief
 @version 0.1
 @date 2024-10-15

 @copyright Copyright (c) 2024

 */

#include "DAQException.h"

/*!
 @brief Construct a new DAQException object
 @param msg The message to be thrown
 */
DAQException::DAQException(const std::string &word, const BlockType_t &type)
    : _msg("")
{
    switch (type)
    {
    case BlockType_t::F_HEADER:
        _msg = "DRSx tag not found: found " + word + " instead\n";
        break;
    case BlockType_t::T_HEADER:
        _msg = "TIME tag not found: found " + word + " instead\n";
        break;
    case BlockType_t::B_HEADER:
        _msg = "B#?? tag not found: found " + word + " instead\n";
        break;
    case BlockType_t::C_HEADER:
        _msg = "C??? tag not found: found " + word + " instead\n";
        break;
    case BlockType_t::E_HEADER:
        _msg = "EHDR tag not found: found " + word + " instead\n";
        break;
    default:
        _msg = "Unknown tag: found " + word + "\n";
        break;
    }
}

/*!
 @brief Return the message to be thrown
 @return const char* The message to be thrown
 */
const char *DAQException::what() const throw()
{
    return _msg.c_str();
}
