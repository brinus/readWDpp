#include "../readWD.hh"

void main4()
{
    DAQFile file;
    WDBEvent event;

    file.Open("../data/testWDB.dat");

    file >> event;

    auto charge = event.GetChannel(0, 0).GetAmplitude();
    cout << charge << endl;
}