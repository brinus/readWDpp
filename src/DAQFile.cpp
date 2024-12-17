#include "DAQFile.h"
#include "DAQReader.h"

DAQFile::DAQFile(const std::string &filename) : _reader(std::make_unique<DAQReader>(filename))
{
};

DAQFile::~DAQFile()
{   
};

void DAQFile::ReadEvent()
{
    _reader->ReadEvent();
};