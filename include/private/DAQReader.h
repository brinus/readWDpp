#ifndef DAQ_READER_H
#define DAQ_READER_H

#include "DAQFile.h"

class DAQFile::DAQReader
{
public:
    DAQReader(DAQFile *const file);
    ~DAQReader() = default;

    bool Read_F_HEADER();
    bool Read_T_HEADER();
    bool Read_B_HEADER();
    bool Read_C_HEADER();
    bool Read_E_HEADER();
    bool Read_T_ARRAY();
    bool Read_V_ARRAY();

private:

    DAQFile *const _parent; ///< Pointer to DAQFile object
    unsigned int _board;    ///< Board type
    unsigned int _channel;  ///< Channel number
};

#endif // DAQ_READER_H