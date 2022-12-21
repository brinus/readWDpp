#include "readWD.hh"
#include <iostream>

using namespace std;

int main(void)
{
    DAQFile file("testDRS.dat");
    TAG tag;
    EventHeader eh;
    file >> tag;
    cout << tag << endl; // DRSx

    file >> tag;
    cout << tag << endl; // TIME

    file >> tag;
    cout << tag << endl; // B#?

    file >> tag;
    cout << tag << endl; // C001

    float times[SAMPLES_PER_WAVEFORM];
    file.Read(times);

    cout << "times..." << endl << endl;

    file >> eh; // EHDR
    cout << eh << endl;

    file >> tag; // B#?
    cout << tag << endl;

    file >> tag; // T#?
    cout << tag << endl;

    file >> tag; // C001
    cout << tag << endl;

    file >> tag; // scaler
    cout << "scaler" << endl;
    cout << "volts..." << endl << endl;

    unsigned short volts[SAMPLES_PER_WAVEFORM];
    file.Read(volts);
}