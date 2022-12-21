#include "readWD.hh"

int main(void)
{
    DAQFile file("../readWD/test_luca.dat");
    DAQEvent event;
    file.Initialise(event);
}