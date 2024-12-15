#ifndef DAQ_READER_H
#define DAQ_READER_H

#include <fstream>
#include <string>

#include "DAQFile.h"
#include "DAQCommon.h"

class DAQFile::DAQReader
{
public:
    DAQReader(const std::string &filename);
    ~DAQReader() = default;

    bool Read_F_HEADER();
    bool Read_T_HEADER();
    bool Read_B_HEADER();
    bool Read_C_HEADER();
    bool Read_E_HEADER();
    bool Read_T_ARRAY();
    bool Read_V_ARRAY();

private:

    bool Initialize();

    std::ifstream   _in;        ///< Input file stream
    std::string     _fileName;  ///< File name
    bool            _init;      ///< Initialization flag
    unsigned int    _board;     ///< Board type
    unsigned int    _channel;   ///< Channel number
    BoardType_t     _boardType; ///< Board type
};

#endif // DAQ_READER_H