#include "../readWD.hh"

#include "TApplication.h"
#include "TGraph.h"

void main4()
{
    DAQFile file;
    DRSEvent event;

    file.Open("../data/testDRS.dat");
    file.GetEvent(100);
    file >> event;
    auto times = event.GetChannel(0, 0).GetTimes();
    auto volts = event.GetChannel(0, 0).GetVolts();

    auto g1 = new TGraph(times.size(), times.data(), volts.data());
    g1->Draw();

    file.Reset();
}

int main(int argc, char **argv)
{
    TApplication app("ROOT Application", &argc, argv);
    main4();
    app.Run();
    return 0;
}