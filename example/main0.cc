#include "../readWD.hh"
#include <iostream>

using namespace std;

int main(void)
{
    DAQFiles files({"test/testDRS.dat", "test/testWDB.dat"});
    for (auto filename : files)
    {
        DAQFile file(filename);
        DAQEvent event;
        file.Initialise(event);
    }
}