#include "../readWD.hh"

using namespace std;

int main(void)
{
    DAQFile file("test/testWDB.dat");
    WDBEvent event;

    file.Initialise(event);
    while (file >> event)
    {

    }
}