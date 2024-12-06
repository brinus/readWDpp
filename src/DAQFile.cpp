#include "readWDpp/DAQFile.h"
#include "private/DAQCommon.h"

DAQFile::DAQFile(const std::string &filename) : _reader(std::make_unique<DAQReader>()),
                                                _in(filename, std::ios::in | std::ios::binary),
                                                _fileName(filename)
{
    if (!_in.is_open())
        throw std::ios_base::failure("ERROR: Could not open file \"" + filename + "\"");

    std::cout << "File \"" << filename << "\" opened successfully\n";
    Initialize();
}

DAQFile::~DAQFile()
{
    if (_in.is_open())
    {
        _in.close();
        std::cout << "File \"" << _fileName << "\" closed\n";
    }
    else
    {
        std::cout << "File \"" << _fileName << "\" was already closed\n";
    }
}

void DAQFile::Initialize()
{
    std::cout << "Initializing file\n";
    _in.seekg(0);
}