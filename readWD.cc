#include "readWD.hh"

using namespace std;

template <class T>
DAQFile &operator>>(DAQFile &file, T &t) // DAQFile >> T
{
    file.Read(t);
    return file;
}

ostream &operator<<(ostream &o, const TAG &tag) // cout << TAG
{
    if (tag.tag[0] == 'B')
    {
        o << tag.tag[0] << tag.tag[1] << *(short *)(tag.tag + 2);
    }
    else if (tag.tag[0] == 'T' and tag.tag[1] == '#')
    {
        o << tag.tag[0] << tag.tag[1] << *(short *)(tag.tag + 2);
    }
    else
    {
        o << tag.tag;
    }
    return o;
}

ostream &operator<<(ostream &o, const EventHeader &eh) // cout << EventHeader
{
    o << eh.tag << endl;
    o << "Date: " << eh.year << "/" << eh.month << "/" << eh.day << endl;
    o << "Hour: " << eh.hour << ":" << eh.min << ":" << eh.sec << "." << eh.ms << endl;
    o << "Range: " << eh.rangeCenter << " Serial number: " << eh.serialNumber;
    return o;
}

void DAQFile::Initialise(DAQEvent &event)
{
    DAQFile &file = *this;
    TAG tag;
    float times[SAMPLES_PER_WAVEFORM];

    file >> tag >> tag; // DRSx - TIME
    while (file >> tag) // B#?
    {
        cout << tag << endl; // print --> B#?
        event.CreateBoard(tag);
        while (file >> tag) // C001
        {
            cout << tag << endl; // print --> C001
            file.Read(times);
            event.Time(times);
        }
        file.ResetTag();
    }
    cout << "Last tag found: " << tag << endl;
    file.ResetTag();
    return;
}

DAQFile::operator bool()
{
    if (in_.eof())
    {
        return 0;
    }
    else if (n_ != 'B' and n_ != 'C')
    {
        return 0;
    }
    o_ = n_;
    return 1;
}

void DAQEvent::Time(float *times)
{
    if (!times_.empty())
    {
        times_.rbegin()->second.push_back(times);
    }
    return;
}

void DAQEvent::CreateBoard(const TAG &tag)
{
    if (strcmp(tag.tag, "EHDR") == 0)
    {
        cout << "Initialization done --> EHDR next" << endl;
        return;
    }
    else if (tag.tag[0] == 'B' and tag.tag[1] == '#')
    {
        times_[tag.tag] = {};
        volts_[tag.tag] = {};
        return;
    }
    cerr << "!! No valid tag passed: expecterd B#?, got " << tag << endl;
    return;
}
