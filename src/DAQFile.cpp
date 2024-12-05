#include "DAQFile.h"
#include "DAQCommon.h"

DAQFile::DAQFile(const std::string& filename) :
	_in(std::ifstream(filename, std::ios::in | std::ios::binary))
{
	if (!_in.is_open()) {
		std::cerr << "ERROR: File \"" << filename << "\" not found\n";
		exit(0);
	}

	std::cout << "File \"" << filename << "\" opened\n";
}

DAQFile::~DAQFile() {
	_in.close();
}