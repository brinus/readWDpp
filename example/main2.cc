#include "../readWD.hh"

#include "TApplication.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"

using namespace std;

void main2(void)
{
    DAQFile file("test/testDRS.dat");
    DRSEvent event;
    int i = 0;
    auto mg = new TMultiGraph();
    auto c1 = new TCanvas("c1", "c1", 1);

    file.Initialise();
    while (file >> event and i < 99)
    {
        if (i == 98)
        {
            auto indices = event.GetChannel(0, 0).GetPeakIndices();
            auto volts = event.GetChannel(0, 0).GetVolts();
            auto times = event.GetChannel(0, 0).GetTimes();
            auto ped = event.GetChannel(0, 0).GetPedestal();

            vector<float> m_volts, m_times;
            for (auto index : indices)
            {
                m_volts.push_back(volts[index]);
                m_times.push_back(times[index]);
            } 

            cout << "Pedestal = " << ped.first << " +/- " << ped.second << endl;
            cout << "#local minima = " << indices.size() << endl;

            auto g1 = new TGraph(SAMPLES_PER_WAVEFORM, times.data(), volts.data());
            auto g2 = new TGraph(indices.size(), m_times.data(), m_volts.data());

            g1->SetMarkerColor(kBlack);
            g1->SetMarkerSize(0.5);
            g1->SetMarkerStyle(kFullSquare);

            g2->SetMarkerColor(kRed);
            g2->SetMarkerSize(0.5);
            g2->SetMarkerStyle(kFullTriangleDown);

            mg->Add(g1, "L");
            mg->Add(g2, "P");
            mg->Draw("A");
            c1->Update();
        }
        ++i;
    }
}

int main(int argc, char **argv)
{
    TApplication app("ROOT Application", &argc, argv);
    main2();
    app.Run();
    return 0;
}