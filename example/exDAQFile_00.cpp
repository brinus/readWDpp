#include <iostream>
#include "DAQFile.h"

int main() {
	std::string fileName = std::string(DATA_DIR) + "/testDRS.dat";
	DAQFile file(fileName);
	return 0;
}