#ifndef DAQFILE_H
#define DAQFILE_H

#include <memory>
#include <string>

class DAQFile
{
public:
    DAQFile(const std::string &filename);
    ~DAQFile();
    DAQFile(const DAQFile &) = delete;
    DAQFile &operator=(const DAQFile &) = delete;

private:
    class DAQReader;
    std::unique_ptr<DAQReader> _reader; ///< Pointer to implementation
};

#endif // DAQFILE_H