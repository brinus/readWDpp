#ifndef DAQFILE_H
#define DAQFILE_H

#include <iostream>
#include <fstream>

#include "../src/DAQCommon.h"

class DAQFile {
public:
	DAQFile(const std::string& filename);
	~DAQFile();

	static const int& GetWaveformLenght() { return WAVEFORMSAMPLE; };

private:
	std::ifstream _in;
};

#endif // DAQFILE_H