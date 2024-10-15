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

    // Define an iterator class
    class Iterator {
    public:
        Iterator(std::ifstream* stream) : stream_(stream) {
            ++(*this); // Read the first line
        }

        std::string operator*() const {
            return current_line_;
        }

        Iterator& operator++() {
            char buffer[4];
            if (stream_ && stream_->read(buffer, 4)) {
                current_line_ = std::string(buffer, 4);
            } else {
                // End of file or error
                stream_ = nullptr;
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return stream_ != other.stream_;
        }

    private:
        std::ifstream* stream_;
        std::string current_line_;
    };

    Iterator begin() {
        return Iterator(&in_);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

private:

    std::ifstream in_;      ///< Input file stream.
    std::string filename_;  ///< File name.
    bool init_;             ///< Flag to check if the file has been initialised.

    BoardType type_; ///< Board type.
    MAP times_;      ///< Array of times.

    DAQFile();

    bool Initialise();
};

#endif