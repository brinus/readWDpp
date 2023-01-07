/*!
 @file readWD.cc
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Definition of methods.
 @version 0.1
 @date 2023-01-05
 
 @copyright Copyright (c) 2023
 
 */
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

const vector<float> *DAQEvent::GetChannel(const int &channel) // this assumes only a board is present
{
    if (volts_.size() > 1)
    {
        if (set_getchannelI_.find(channel) == set_getchannelI_.end())
        {
            cerr << "!! Error: more than a board found --> Use GetChannel(int board, int channel)" << endl;
            set_getchannelI_.insert(channel);
        }
        return NULL;
    }

    int i = 0;
    for (auto &[bKey, bVal] : volts_)
    {
        if (bVal.size() - 1 < channel)
        {
            if (set_getchannelI_.find(channel) == set_getchannelI_.end())
            {
                cerr << "!! Error: channel number provided not found --> Exceded maximum channel number" << endl
                     << "       passed channel number: " << channel << endl
                     << "       volts[" << bKey << "].size() = " << bVal.size() << endl;
                set_getchannelI_.insert(channel);
            }
            return NULL;
        }
        for (auto &[cKey, cVal] : bVal)
        {
            if (i == channel)
            {
                if (set_getchannelS_.find(bKey + cKey) == set_getchannelS_.end())
                {
                    cout << "Obtaining data for board: " << bKey << " channel: " << cKey << endl;
                    set_getchannelS_.insert(bKey + cKey);
                }
                return &cVal;
            }
            ++i;
        }
    }
    return NULL;
}

const vector<float> *DAQEvent::GetChannel(const int &board, const int &channel)
{
    if (board > volts_.size() - 1)
    {
        if (set_getChannelII_.find(to_string(board) + "-" + to_string(channel)) == set_getChannelII_.end())
        {
            cerr << "!! Error: board number provided not found --> Exceded maximum board number" << endl
                 << "       passed board number: " << board << endl
                 << "       volts.size() = " << volts_.size() << endl;
            set_getChannelII_.insert(to_string(board) + "-" + to_string(channel));
        }
        return NULL;
    }

    int i = 0, j = 0;
    for (auto &[bKey, bVal] : volts_)
    {
        if (i == board)
        {
            if (bVal.size() - 1 < channel)
            {
                if (set_getChannelII_.find(to_string(board) + "-" + to_string(channel)) == set_getChannelII_.end())
                {
                    cerr << "!! Error: channel number provided not found --> Exceded maximum channel number" << endl
                         << "       passed channel number: " << channel << endl
                         << "       volts[" << bKey << "].size() = " << bVal.size() << endl;
                    set_getChannelII_.insert(to_string(board) + "-" + to_string(channel));
                }
                return NULL;
            }
            for (auto &[cKey, cVal] : bVal)
            {
                if (j == channel)
                {
                    if (set_getchannelS_.find(bKey + cKey) == set_getchannelS_.end())
                    {
                        cout << "Obtaining data for board: " << bKey << " channel: " << cKey << endl;
                        set_getchannelS_.insert(bKey + cKey);
                    }
                    return &cVal;
                }
                ++j;
            }
        }
        ++i;
    }
    return NULL;
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

/*!
     @brief Function to perform the time calibration.
     
     @details The time calibration is performed as specified in the DRS manual.
 
     @param tCell Cell number at which the signal triggered the board.
 */
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
                for_each(channel.begin(), channel.end(), [&](float &val)
                         { val += dt; });
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
    cout << bTag;
    if (bTag.tag[3] == '8')
    {
        cout << " --> WaveDREAM Board" << endl;
    }
    else
    {
        cout << " --> DRS Evaluation Board" << endl;
    }

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
    cout << "Closing file " << filename_ << "..." << endl;
    in_.close();
    return;
}

/*!
 @brief Read into a @ref TAG.
 
 @param t 
 @return true 
 @return false 
 */
bool DAQFile::operator>>(TAG &t) // DAQFile >> TAG
{
    if (!in_.good())
    {
        return 0;
    }
    this->Read(t);
    return *this;
}

/*!
 @brief Read into a @ref EventHeader.
 
 @param eh 
 @return true 
 @return false 
 */
bool DAQFile::operator>>(EventHeader &eh) // DAQFile >> EventHeader
{
    if (!in_.good())
    {
        return 0;
    }
    this->Read(eh);
    return *this;
}

/*!
 @brief Read into a @ref DRSEvent.
 
 @param event 
 @return true 
 @return false 
 */
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
    if (eh.serialNumber == 1)
    {
        cout << "Event serial number: " << eh.serialNumber << endl;
    }
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

/*!
 @brief Read into a @ref WDBEvent.
 
 @param event 
 @return true 
 @return false 
 */
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
