#include "../readWD.hh"
#include <iostream>

using namespace std;

int main(void)
{
    DAQFiles files({"test/testDRS.dat", "test/testWDB.dat"});
    for (auto &file : files)
    {
        DAQEvent event;
        file->Initialise(event);
        file->Close();
    }

    // What happends if I iterate again? It generates an error --> to be solved
    for (auto &file : files)
    {
        DAQEvent event;
        file->Initialise(event);
    }
}