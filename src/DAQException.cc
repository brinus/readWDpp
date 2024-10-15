/*!
 @file DAQException.cc
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief
 @version 0.1
 @date 2024-10-15

 @copyright Copyright (c) 2024

 */

#include "DAQException.hh"

/*!
 @brief Construct a new DAQException object
 @param msg The message to be thrown
 */
DAQException::DAQException(const std::string &word, FileTag tag)
{
    switch (tag)
    {
    case F_HEADER:
        msg_ = "DRSx tag not found: " + word;
        msg_ += "\nInitialisation aborted.";
        break;
    case T_HEADER:
        msg_ = "TIME tag not found: " + word;
        msg_ += "\nInitialisation aborted.";
        break;
    case B_HEADER:
        msg_ = "B#?? tag not found: " + word;
        break;
    case C_HEADER:
        msg_ = "C??? tag not found: " + word;
        break;
    case E_HEADER:
        msg_ = "EHDR tag not found: " + word;
        break;
    default:
        msg_ = "Unknown tag: " + word;
        break;
    }
}

/*!
 @brief Return the message to be thrown
 @return const char* The message to be thrown
 */
const char *DAQException::what() const throw()
{
    return msg_.c_str();
}