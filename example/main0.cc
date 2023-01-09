#include "../readWD.hh"
#include "TH1F.h"

using namespace std;

int main(void)
{
    DAQFile file("test/testDRS.dat");
    DRSEvent event;
    TH1F *h1 = new TH1F("Pedestal mean", "h1", 100, -0.002, 0.004);
    TH1F *h2 = new TH1F("Pedestal std.dev.", "h2", 100, -0.002, 0.004);

    h1->SetLineColor(kRed);
    h2->SetLineColor(kGreen);

    file.Initialise(event);
    while(file >> event)
    {
        auto pedestal = event.GetChannel("B#2440", "C001").GetPedestal();
        h1->Fill(pedestal.first);
        h2->Fill(pedestal.second);
    }

    h2->Draw();
    h1->Draw("SAMES");
}

void main0()
{
    main();
}