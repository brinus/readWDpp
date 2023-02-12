/*!
 @example main3.cc
 
 The WDB has at the inside two different DRS chips, due to this fact the difference between two same timecells depends on what channel are we looking at.
 In particular the channels going from 0 to 7 (included) are on the same first chip, the channels going from 8 to 15 are on the same second chip.

 To show this effect a NIM signal is generated and splitted equally to be then distributed to two channels of the WDB. The file ```data/split1-2at4GSPS``` contains 
 the waveform on channel 0 and channel 1, the file ```data/split1-8at4GSPS``` contains the waveform on the channel 0 and channel 8. 

 This script uses the methods @ref DAQEvent::GetChannel and @ref DAQEvent::GetTimes() to get the times array of the selected channels at every event reading. 
 Using a simple for loop we get each cell difference and we then fill an histogram.

 The results show that there is an order of magnitude in temporal resolution between channels on the same chip (red dotted line, *std.dev.* \f$\sim 10^{-12}\textup{s}\f$) and channel 
 on different chips (blue solid line, *std.dev.* \f$\sim 10^{-11}\textup{s}\f$). This effect can anyway be corrected using the information contained on channels 16 and 17. 
 */

#include "../readWD.hh"

#include "TApplication.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TH1F.h"

using namespace std;

void main1()
{
    DAQFile file;
    WDBEvent event;

    auto c0 = new TCanvas("c0", "c0", 1);
    auto h0 = new TH1F("ch01", "ch01", 80, -0.07e-9, 0.07e-9);
    auto h1 = new TH1F("ch08", "ch08", 80, -0.7e-9, 0.7e-9);
    auto legend = new TLegend();

    file.Open("data/split1-2at4GSPS");
    file.Initialise();

    while (file >> event)
    {
        auto &times0 = event.GetChannel(0, 0).GetTimes();
        auto &times1 = event.GetChannel(0, 1).GetTimes();

        for (int j = 0; j < SAMPLES_PER_WAVEFORM; ++j)
        {
            h0->Fill(times0[j] - times1[j]);
        }
    }

    file.Close();
    file.Open("data/split1-8at4GSPS");
    file.Initialise();

    while (file >> event)
    {
        auto &times0 = event.GetChannel(0, 0).GetTimes();
        auto &times8 = event.GetChannel(0, 8).GetTimes();
        
        for (int j = 0; j < SAMPLES_PER_WAVEFORM; ++j)
        {
            h1->Fill(times0[j] - times8[j]);
        }
    }

    h1->SetTitle("Time resolution on same chip and on different chips");
    h1->SetLineColor(4);

    h0->SetLineColor(2);
    h0->SetLineStyle(2);

    h1->Draw();
    h0->Draw("SAMES");

    h1->GetYaxis()->SetRangeUser(0, 5.5e4);
    h1->GetXaxis()->SetTitle("Time [s]");

    legend->AddEntry(h0, "same chip");
    legend->AddEntry(h1, "different chip");
    
    legend->Draw();
    c0->Update();

    return;
}

int main(int argc, char **argv)
{
    TApplication app("ROOT Application", &argc, argv);
    main1();
    app.Run();
    return 0;
}