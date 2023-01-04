#include "readWD.hh"

using namespace std;

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

void DAQEvent::SetTrigger(const unsigned short &trig)
{
    this->TimeCalibration(trig);
    trigger_ = trig;
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

void DAQEvent::SetVolts(const TAG &tBoard, const TAG &tChannel, const vector<float> &volts)
{
    if (!volts_.empty())
    {
        if (!volts_.rbegin()->second.empty())
        {
            string board{"B#" + to_string(*(short *)(tBoard.tag + 2))};
            string channel{tChannel.tag};
            volts_[board][channel] = std::move(volts);
        }
    }
    return;
}

void DAQEvent::TimeCalibration(const unsigned short &tCell)
{
    float t1, t2, dt;

    // Time sum
    for (auto &[board_tag, board] : times_)
    {
        for (auto &[channel_tag, times] : board)
        {
            rotate(times.begin(), times.begin() + tCell, times.end());
            partial_sum(times.begin(), times.end(), times.begin());
        }
    }

    // Time alignement
    for (auto &[board_tag, board] : times_)
    {
        t1 = -1.;
        for (auto &[channel_tag, channel] : board)
        {
            if (t1 == -1.)
            {
                t1 = channel[(1024 - tCell) % 1024];
            }
            else
            {
                t2 = channel[(1024 - tCell) % 1024];
                dt = t1 - t2;
                for_each(channel.begin(), channel.end(), [&](float& val){val += dt;});
            }
        }
    }
}

void DAQEvent::CreateBoard(const TAG &tag)
{
    if (tag.tag[0] == 'B' and tag.tag[1] == '#')
    {
        string board{"B#" + to_string(*(short *)(tag.tag + 2))};
        times_[board] = {};
        times_corr_[board] = {};
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
        times_corr_[board][channel] = {};
        volts_[board][channel] = {};
        return;
    }
    cerr << "!! No valid tag passed: expecterd C, got " << tag << endl;
    return;
}

void DAQFile::Initialise(DAQEvent &event)
{
    if (!in_.is_open())
    {
        cerr << "!! Error: file not open --> use DAQFile(filename)" << endl;
        return;
    }

    if (in_.tellg() != 0)
    {
        cerr << "!! Error: file already initialized --> ???" << endl;
        return;
    }

    DAQFile &file = *this;
    TAG bTag, cTag;
    vector<float> times(SAMPLES_PER_WAVEFORM);

    cout << "Initializing file " << filename_ << endl;

    file >> bTag; // DRSx
    file >> cTag; // TIME
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

void DAQFile::Read(EventHeader &eh)
{
    in_.read((char *)&eh, sizeof(eh));
    n_ = eh.tag[0];
}

void DAQFile::Read(vector<float> &vec)
{
    for (int i = 0; i < vec.size(); ++i)
    {
        in_.read((char *)&vec.at(i), sizeof(float));
    }
    return;
}

void DAQFile::Read(vector<unsigned short> &vec)
{
    for (int i = 0; i < vec.size(); ++i)
    {
        in_.read((char *)&vec.at(i), sizeof(unsigned short));
    }
    return;
}

void DAQFile::Close()
{
    in_.close();
    return;
}

bool DAQFile::operator>>(TAG &t) // DAQFile >> TAG
{
    if (!in_.good())
    {
        return 0;
    }
    this->Read(t);
    return *this;
}

bool DAQFile::operator>>(EventHeader &eh) // DAQFile >> EventHeader
{
    if (!in_.good())
    {
        return 0;
    }
    this->Read(eh);
    return *this;
}

bool DAQFile::operator>>(DRSEvent &event) // DAQFile >> DRSEvent
{
    if (!in_.good())
    {
        return 0;
    }

    DAQFile &file = *this;
    TAG bTag, cTag, tag;
    EventHeader eh;
    vector<unsigned short> volts(SAMPLES_PER_WAVEFORM);
    vector<float> volts_corr(SAMPLES_PER_WAVEFORM);

    // Read only one event
    file >> eh;
    if (eh.serialNumber % 100 == 0)
    {
        cout << "Event serial number: " << eh.serialNumber << endl;
    }
    while (file >> bTag)
    {
        file >> tag; // Trigger cell
        event.SetTrigger(*(unsigned short *)(tag.tag + 2));
        while (file >> cTag)
        {
            file >> tag; // Time scaler
            file.Read(volts);
            for (int i = 0; i < volts.size(); ++i)
            {
                volts_corr[i] = volts[i] / 65536. + eh.rangeCenter / 1000. - 0.5;
            }
            event.SetVolts(bTag, cTag, volts_corr);
        }
        file.ResetTag();
    }
    file.ResetTag();
    return 1;
}

bool DAQFile::operator>>(WDBEvent &event) // DAQFile >> WDBEvent
{
    if (!in_.good())
    {
        return 0;
    }

    DAQFile &file = *this;
    TAG bTag, cTag, tag;
    EventHeader eh;
    vector<unsigned short> volts(SAMPLES_PER_WAVEFORM);
    vector<float> volts_corr(SAMPLES_PER_WAVEFORM);

    // Read only one event
    file >> eh;
    if (eh.serialNumber % 100 == 0)
    {
        cout << "Event serial number: " << eh.serialNumber << endl;
    }
    while (file >> bTag)
    {
        while (file >> cTag)
        {
            file >> tag; // Time scaler
            file >> tag; // Trigger cell
            event.SetTrigger(*(unsigned short *)(tag.tag + 2));
            file.Read(volts);
            for (int i = 0; i < volts.size(); ++i)
            {
                volts_corr[i] = volts[i] / 65536. + eh.rangeCenter / 1000. - 0.5;
            }
            event.SetVolts(bTag, cTag, volts_corr);
        }
        file.ResetTag();
    }
    file.ResetTag();
    return 1;
}

DAQFile::operator bool()
{
    map<char, char> header{{'E', 'B'}, {'B', 'C'}, {'C', 'B'}};
    if (!in_.good())
    {
        cout << "End of file reached" << endl;
        return 0;
    }
    else if (n_ == 'T' or n_ == 'D') // Ignores DRSx and TIME
    {
        return 0;
    }
    else if (n_ == o_) // C --> C, B --> B
        return 1;
    else if (n_ == header[o_]) // E --> B, B --> C, C --> B
    {
        o_ = n_;
        return 1;
    }
    else if (n_ == 'E' and initialization_ == 0) // End of initialization, first EHDR
    {
        cout << "Initialization done --> EHDR next" << endl;
        initialization_ = 1;
        return 0;
    }
    return 0;
}
