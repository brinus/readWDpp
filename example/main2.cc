#include "../readWD.hh"

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TLine.h"
#include "TPad.h"

using namespace std;
using MAP = map<string, map<string, vector<float>>>;

void main2()
{
    DAQFile file("test/testWDB3.bin");
    WDBEvent event;
    int i = 0;
    int evt = 146;

    file.Initialise(event);
    while (file >> event and i < evt)
    {
        if (i == evt - 1)
        {
            auto times = event.GetChannel(0, 0).GetTimes();
            auto volts = event.GetChannel(0, 0).GetVolts();
            auto ped = event.GetChannel(0, 0).GetPedestal();
            auto iw = event.GetChannel(0, 0).GetIntegrationBounds();

            cout << "Integration index: " << iw.first << " " << iw.second << endl;
            cout << "Pedestal: " << ped.first << " +/- " << ped.second << endl;

            TCanvas *c1 = new TCanvas("c1", "c1", 1);
            c1->cd();

            TGraph *g1 = new TGraph(times.size(), times.data(), volts.data());
            string title = "{event}=" + to_string(evt) + ", {iw}=(" + to_string(iw.first) + "," + to_string(iw.second) + "), {ped}=" + to_string(ped.first) + "+/-" + to_string(ped.second);
            g1->SetMarkerStyle(kFullSquare);
            g1->SetMarkerSize(0.2);
            g1->SetTitle(title.c_str());
            g1->Draw("AP");

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
    }

    // file.Close();

    // TCanvas *c2 = new TCanvas("c2", "c2", 1);
    // c2->cd();
    // TH1F *h1 = new TH1F("h1", "charge histogram", 100, 0, 15);

    // file.Open("test/testDRS.dat");
    // file.Initialise(event);
    // while (file >> event)
    // {
    //     auto charge = event.GetChannel(0, 0).GetCharge();
    //     h1->Fill(-charge);
    //     ++i;
    // }

    // h1->Draw();
    // c2->Update();

    return;
}

int main(int argc, char **argv)
{
    TApplication app("ROOT Application", &argc, argv);
    main2();
    app.Run();
    return 0;
}