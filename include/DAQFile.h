#ifndef DAQFILE_H
#define DAQFILE_H

#include <iostream>
#include <fstream>

class DAQFile {
public:
	DAQFile(const std::string& filename);
	~DAQFile();

private:
	std::ifstream _in;
};

#endif // DAQFILE_H