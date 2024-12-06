#ifndef DAQFILE_H
#define DAQFILE_H

#include <iostream>
#include <fstream>
#include <memory>
#include <string>

enum class BoardType_t : unsigned short;

class DAQFile
{
public:
	DAQFile(const std::string &filename);
	~DAQFile();
	DAQFile(const DAQFile &) = delete;
	DAQFile &operator=(const DAQFile &) = delete;

private:
	bool Initialize();

	class DAQReader;
	std::unique_ptr<DAQReader> _reader; ///< Unique pointer to DAQReader object

	std::ifstream _in;		///< Input file stream
	std::string _fileName;	///< File name
	BoardType_t _boardType; ///< Board type
	bool _init;				///< Initialization flag
};

#endif // DAQFILE_H