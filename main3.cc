#include "readWD.hh"
#include <vector>
#include <string>

#include "TH1F.h"

using namespace std;

int main(int argc, char **argv)
{
    vector<string> nfiles(argc - 1);

    for (int i = 0; i < argc - 1; ++i)
    {
        nfiles[i] = argv[i+1];
    }

    TH1F *h = new TH1F();

    DAQFiles files(nfiles);

    for (string &filename : files)
    {
        DAQFile file(filename);
        DAQEvent evt;
        file.Initialise(evt);
        while (file >> evt)
        {
            float charge = evt.GetChannel(0).GetCharge();
            h->Fill(charge);
        }
    }
}