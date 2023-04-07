#include "../readWD.hh"

void main4()
{
    DAQFile file;
    WDBEvent event;

    file.Open("data/testWDB.dat");

    event.MakeConfig(file);
    event.ShowConfig();
}