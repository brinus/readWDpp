#include "../readWD.hh"
#include "TGraph.h"

using namespace std;

int main(void)
{
    DAQFile file("test/testWDB.dat");
    WDBEvent event;
    int i = 0;

    file.Initialise(event);
    while (file >> event and i < 10)
    {
        if (i == 9)
        {
        auto times = event.GetTimeMap();
        auto volts = event.GetVoltMap();

        vector<float> tt = times["B#39"]["C000"];
        vector<float> vv = volts["B#39"]["C000"];

        cout << tt.size() << " " << vv.size();

        TGraph * g1 = new TGraph(tt.size(), tt.data(), vv.data());
        g1->SetMarkerStyle(7);
        g1->Draw("AP");

        }
        ++i;
    }
}

void main3()
{
    main();
}