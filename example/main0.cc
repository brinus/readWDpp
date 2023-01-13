/*!
 @example main0.cc
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Example on how to use @ref DAQEvent::GetPedestal()
 @version 0.1
 @date 2023-01-09
 
 @copyright Copyright (c) 2023
 
 @details The method @ref DAQFile::operator>> is used to fill at each event the instance of @ref DAQEvent.
 In the `while` the object `event` is updated everytime with the new event read from the file. The the proper channel is
 chosen with @ref DAQEvent::GetChannel() and in cascade is used @ref DAQEvent::GetPedestal() that returns an 
 `std::pair<float, float>`, the two values are the mean and the standard deviation of the pedestal, these values are 
 used to fill two distinct histograms using ROOT. 
 
 */

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
        auto pedestal = event.GetChannel(0, 0).GetPedestal();
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
