/*!
 @example main1.cc

 This example shows many utilities. The file ```testDRS.dat``` is read, three pads are made to contain:
 1. An event selected by the variable ```evt```.
 2. The charge histogram.
 3. The amplitude histogram 
 
 In details, the first pad contains the selected waveform, the pedestal band (red lines), the integration window (green lines) and the peak (red triangle).
 The integration window in this example is setted by the user with the method @ref DAQEvent::SetIntWindow().

 */

#include "../readWD.hh"

#include "TApplication.h"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TLine.h"
#include "TPad.h"
#include "TPoint.h"

using namespace std;

void main1()
{
    TCanvas *c1 = new TCanvas("c1", "c1", 600, 750);
    auto *pad1 = new TPad("pad1", "waveform", 0, 0.5, 1, 1);
    auto *pad2 = new TPad("pad2", "charge", 0, 0, 0.5, 0.5);
    auto *pad3 = new TPad("pad3", "amplitude", 0.5, 0, 1, 0.5);

    TH1F *h1 = new TH1F("h1", "charge histogram", 100, 0, 10);
    TH1F *h2 = new TH1F("h2", "amplitude histogram", 100, -0.5, 0);

    pad1->Draw();
    pad2->Draw();
    pad3->Draw();

    DAQFile file("data/testDRS.dat");
    DRSEvent event;
    
    int i = 0;
    int evt = 100;
    float iw_start = 400;
    float iw_stop = 600;

    file.Initialise();
    while (file >> event)
    {
        event.GetChannel(0, 0).SetIntWindow(iw_start, iw_stop);
        auto charge = event.GetChannel(0, 0).GetCharge();
        auto amplitude = event.GetChannel(0, 0).GetAmplitude();
        if (i == evt or i == 9999)
        {
            cout << "Charge: " <<  charge << endl;

            auto times = event.GetChannel(0, 0).GetTimes();
            auto volts = event.GetChannel(0, 0).GetVolts();
            auto ped = event.GetChannel(0, 0).GetPedestal();
            auto iw = event.GetChannel(0, 0).GetIntegrationBounds();
            auto indexMin = event.GetChannel(0, 0).GetPeakIndices();

            cout << "Integration index: " << iw.first << " " << iw.second << endl;
            cout << "Pedestal: " << ped.first << " +/- " << ped.second << endl;

            auto mg = new TMultiGraph();
            string title = "{event}=" + to_string(evt) + ", {iw}=(" + to_string(iw.first) + "," + to_string(iw.second) + "), {ped}=" + to_string(ped.first) + "+/-" + to_string(ped.second);
            mg->SetTitle(title.c_str());

            auto g1 = new TGraph(times.size(), times.data(), volts.data());

            auto g2 = new TGraph(0);
            g2->AddPoint(times[indexMin[0]], volts[indexMin[0]]);
            g2->SetMarkerSize(0.8);
            g2->SetMarkerColor(kRed);
            g2->SetMarkerStyle(kFullTriangleDown);

            pad1->cd();
            mg->Add(g1, "L");
            mg->Add(g2, "P");
            mg->Draw("A");

            c1->Update();
            TLine *ped_m = new TLine(gPad->GetUxmin(), ped.first - 5 * ped.second, gPad->GetUxmax(), ped.first - 5 * ped.second);
            TLine *ped_M = new TLine(gPad->GetUxmin(), ped.first + 5 * ped.second, gPad->GetUxmax(), ped.first + 5 * ped.second);
            TLine *iw_m = new TLine(times[iw.first], gPad->GetUymin(), times[iw.first], gPad->GetUymax());
            TLine *iw_M = new TLine(times[iw.second], gPad->GetUymin(), times[iw.second], gPad->GetUymax());

            ped_m->SetLineColor(kRed);
            ped_M->SetLineColor(kRed);
            iw_m->SetLineColor(kGreen);
            iw_M->SetLineColor(kGreen);

            iw_m->Draw("SAME");
            iw_M->Draw("SAME");
            ped_m->Draw("SAME");
            ped_M->Draw("SAME");

            c1->Update();
        }
        ++i;

        h1->Fill(charge);
        h2->Fill(amplitude);
    }

    pad2->cd();
    h1->Draw();
    pad3->cd();
    h2->Draw();
    c1->Update();

    return;
}

int main(int argc, char **argv)
{
    TApplication app("ROOT Application", &argc, argv);
    main1();
    app.Run();
    return 0;
}