#include <iostream>
#include "DAQFile.h"

int main() {
	std::string fileName = std::string(DATA_DIR) + "/testWDB.dat";
	DAQFile file(fileName);

	file.ReadEvent();
	return 0;
}