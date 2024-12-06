#ifndef DAQFILE_H
#define DAQFILE_H

#include <iostream>
#include <fstream>
#include <memory>

#include "../src/private/DAQCommon.h"

class DAQFile
{
public:
	DAQFile(const std::string &filename);
	~DAQFile();

	static const int &GetWaveformLenght() { return WAVEFORMSAMPLE; };

private:
	void Initialize();

	class DAQReader
	{
	public:
		explicit DAQReader() = default;
		~DAQReader() = default;

		template <typename T>
		void Read(T *data);
	};

	std::unique_ptr<DAQReader> _reader; ///< Unique pointer to DAQReader object
	std::ifstream _in;					///< Input file stream
	std::string _fileName;				///< File name
};

#endif // DAQFILE_H