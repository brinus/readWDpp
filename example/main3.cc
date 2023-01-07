#include "../readWD.hh"
#include "TGraph.h"

using namespace std;

int main(void)
{
    DAQFile file("test/testWDB.dat");
    WDBEvent Wevent;
    int i = 0;

    file.Initialise(Wevent);
    while (file >> Wevent)
    {
        auto vec0 = Wevent.GetChannel(0);
        ++i;
    }

    DRSEvent Devent;
    i = 0;
    file.Close();
    file.Open("test/testDRS.dat");
    file.Initialise(Devent);
    while (file >> Devent)
    {
        ++i;
        auto vec1 = Devent.GetChannel(0);
        auto vec2 = Devent.GetChannel(0, 0); // Shouldn't print anything
        auto vec3 = Devent.GetChannel(1);    // Should give error
        auto f = Devent.GetChannel(0).GetCharge();
    }
}

void main3()
{
    main();
}