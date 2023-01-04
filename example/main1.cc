#include "../readWD.hh"

using namespace std;
using MAP = map<string, map<string, vector<float>>>;

int main(void)
{
    DAQFile file("test/testWDB.dat");
    WDBEvent event;
    file.Initialise(event);

    // Let's see what's in TIME

    cout << endl
         << " --- main1.cpp ---" << endl;

    MAP times = event.GetTimeMap();

    cout << "times.size() = " << times.size() << endl;
    for (auto& [board_key, board] : times)
    {
        cout << board_key << endl;
        for (auto& [channel_key, channel] : board)
        {
            cout << channel_key << endl;
            int i = 0;
            for (auto& val : channel)
            {
                if ( i < 5 )
                {
                    cout << val << endl;
                }
                ++i;

            }
        }
    }
    return 0;
}