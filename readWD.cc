#include "readWD.hh"

using namespace std;

DAQFile &operator>>(DAQFile &file, TAG &t) // DAQFile >> T
{
    file.Read(t);
    return file;
}

DAQFile &operator>>(DAQFile &file, EventHeader &eh) // DAQFile >> T
{
    file.Read(eh);
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

DAQFile::operator bool()
{
    map<char, char> header{{'E', 'B'},{'B', 'C'},{'C','E'}};
    if (in_.eof())
    {
        return 0;
    }
    else if (n_ != 'B' and n_ != 'C' and n_ != 'E')
    {
        cerr << "!! Error --> Unexpected tag" << endl;
        return 0;
    }
    else if (n_ == o_) return 1;
    else if (n_ == header[o_])
    {
        if (n_ == 'E' and !initialization_)
        {
            cout << "Initialization done --> EHDR next" << endl;
            initialization_ = 1;
            o_ = 'B';
            return 0;
        }
        o_ = n_;
        return 1;
    }
    else if (o_ == 'C' and n_ == 'B')
    {
        o_ = n_;
        return 1;
    }
    return 0;
}

void DAQEvent::SetTime(const vector<float> &times)
{
    if (!times_.empty())
    {
        if (!times_.rbegin()->second.empty())
        {
            times_.rbegin()->second.rbegin()->second = std::move(times);
        }
    }
    return;
}

void DAQEvent::CreateBoard(const TAG &tag)
{
    if (tag.tag[0] == 'B' and tag.tag[1] == '#')
    {
        string board{"B#" + to_string(*(short *)(tag.tag + 2))};
        times_[board] = {};
        volts_[board] = {};
        return;
    }
    cerr << "!! Error: no valid tag passed --> expecterd B#?, got " << tag << endl;
    return;
}

void DAQEvent::CreateChannel(const TAG &tBoard, const TAG &tag)
{
    string board{"B#" + to_string(*(short *)(tBoard.tag + 2))};
    if (times_.find(board) == times_.end())
    {
        cerr << "!! Error: invalid board for this channel --> " << board << endl;
        return;
    }
    if (tag.tag[0] == 'C')
    {
        string channel{tag.tag};
        times_[board][channel] = {};
        volts_[board][channel] = {};
        return;
    }
    cerr << "!! No valid tag passed: expecterd C, got " << tag << endl;
    return;
}

void DAQFile::Initialise(DAQEvent &event)
{
    if (not in_.is_open())
    {
        cerr << "!! Error: file not open --> use DAQFile(filename)" << endl;
        return;
    }

    DAQFile &file = *this;
    TAG bTag, cTag;
    vector<float> times(SAMPLES_PER_WAVEFORM, -1);

    cout << "Initializing file " << filename_ << endl;

    file >> bTag >> cTag; // DRSx - TIME
    cout << bTag << endl
         << cTag << endl;
    
    o_ = 'B';
    initialization_ = 0;
    while (file >> bTag) // B#?
    {
        cout << bTag << ":" << endl; // print --> B#?
        event.CreateBoard(bTag);
        while (file >> cTag) // C001
        {
            cout << " --> " << cTag << endl; // print --> C001
            event.CreateChannel(bTag, cTag);
            file.Read(times);
            event.SetTime(times);
        }
        file.ResetTag();
    }
    file.ResetTag();
    return;
}

void DAQFile::Read(vector<float> &vec)
{
    for (int i = 0; i < vec.size(); ++i)
    {
        in_.read((char *)&vec.at(i), sizeof(float));
    }
    return;
}

DAQFiles::DAQFiles(const vector<string> &filenames)
{
    filenames_ = filenames;
    for (auto filename : filenames)
    {
        DAQFile * file = new DAQFile(filename);
        files_.push_back(file);
    }
}