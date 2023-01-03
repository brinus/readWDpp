#include "../readWD.hh"

using namespace std;
using MAPv = std::map<std::string, std::map<std::string, std::vector<float>>>;

int main(void)
{
    DAQFile file("test/testWDB.dat");
    WDBEvent event;

    file.Initialise(event);
    while (file >> event)
    {

    }
}