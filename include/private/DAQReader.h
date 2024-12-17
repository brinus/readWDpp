#ifndef DAQ_READER_H
#define DAQ_READER_H

#include <fstream>
#include <string>
#include <vector>

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

    void ReadEvent();

private:

    bool Initialize();

    std::ifstream           _in;            ///< Input file stream
    std::string             _fileName;      ///< File name
    bool                    _init;          ///< Initialization flag
    int                     _board;         ///< Board type
    int                     _channel;       ///< Channel number
    BoardType_t             _boardType;     ///< Board type
    std::map<int, Board_t>  _boardMap;      ///< Board list
    EventHeader_t           _eventHeader;   ///< Event header
};

#endif // DAQ_READER_H