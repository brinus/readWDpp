#include "../readWD.hh"

using namespace std;
using MAP = map<string, map<string, vector<float>>>;

int main(void)
{
    DAQFile file("test/testWDB.dat");
    DAQEvent event;
    file.Initialise(event);

    // Let's see what's in TIME

    cout << endl
         << " --- main1.cpp ---" << endl;

    MAP times = event.GetTimeMap();

    cout << "times.size() = " << times.size() << endl;
    for (auto it1 : times)
    {
        cout << it1.first << ":" << endl;
        cout << "times[" << it1.first << "].size() = " << it1.second.size() << endl;
        for (auto it2 : it1.second)
        {
            cout << " --> " << it2.first << " : " << it2.second.size() << endl;
            for (int i = 0; i < it2.second.size(); i += 100)
            {
                cout << i << ": " << it2.second.at(i) << endl;
            }
        }
    }
    return 0;
}