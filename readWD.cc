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

DAQEvent &DAQEvent::GetChannel(const int &board, const int &channel)
{
    int i = 0, j = 0;
    for (auto &[bKey, bVal] : volts_)
    {
        if (i == board)
        {
            if (bVal.size() - 1 < channel)
            {
                return *this;
            }
            for (auto &[cKey, cVal] : bVal)
            {
                if (j == channel)
                {
                    wfVolts_ = cVal;
                    wfTimes_ = times_[bKey][cKey];
                    is_getch_ = true;
                    return *this;
                }
                ++j;
            }
        }
        ++i;
    }
    return *this;
}

void DAQEvent::EvalPedestal()
{
    if (!is_ped_)
    {
        is_ped_ = true;
        ped_ = {0., 0.};
        ped_.first = accumulate(wfVolts_.begin(), wfVolts_.begin() + 100, 0.) / 100;
        for (int i = 0; i < 100; ++i)
        {
            ped_.second += pow(wfVolts_[i] - ped_.first, 2);
        }
        ped_.second = sqrt(ped_.second / 100);
    }

    return;
}

const pair<float, float> &DAQEvent::GetPedestal()
{
    try
    {
        if (is_getch_)
        {
            (*this).EvalPedestal();
            is_getch_ = false;
            return ped_;
        }
        else
        {
            throw runtime_error("!! Error: select a channel --> Use DAQEvent::GetChannel()");
        }
    }
    catch(exception &obj)
    {
        cerr << obj.what() << endl;
        exit(0);
    }
}

const vector<float> &DAQEvent::GetVolts()
{
    try
    {
        if (is_getch_)
        {
            is_getch_ = false;
            return wfVolts_;
        }
        else
        {
            throw runtime_error("!! Error: select a channel --> Use DAQEvent::GetChannel()");
        }
    }
    catch(exception &obj)
    {
        cerr << obj.what() << endl;
        exit(0);
    }
}

const vector<float> &DAQEvent::GetTimes()
{
    try
    {
        if (is_getch_)
        {
            is_getch_ = false;
            return wfTimes_;
        }
        else
        {
            throw runtime_error("!! Error: select a channel --> Use DAQEvent::GetChannel()");
        }
    }
    catch(exception &obj)
    {
        cerr << obj.what() << endl;
        exit(0);
    }
}

/*!
     @brief Function to perform the time calibration.

     @details The time calibration is performed as specified in the DRS manual.

     @param tCell Cell number at which the signal triggered the board.
 */
void DAQEvent::TimeCalibration(const unsigned short &tCell, const std::vector<float> &times, int i, int j)
{
    times_[i][j] = times;
    vector<float> &times_ij = times_[i][j];
    rotate(times_ij.begin(), times_ij.begin() + tCell, times_ij.end());
    partial_sum(times_ij.begin(), times_ij.end(), times_ij.begin());

    return;

    // float t1, t2, dt;

    // // Time sum: DA CORREGGERE!
    // for (auto &[board_tag, board] : times_)
    // {
    //     for (auto &[channel_tag, times] : board)
    //     {
    //         rotate(times.begin(), times.begin() + tCell, times.end());
    //         partial_sum(times.begin(), times.end(), times.begin());
    //     }
    // }

    // Time alignement
    // for (auto &[board_tag, board] : times_)
    // {
    //     t1 = -1.;
    //     for (auto &[channel_tag, channel] : board)
    //     {
    //         if (t1 == -1.)
    //         {
    //             t1 = channel[(1024 - tCell) % 1024];
    //         }
    //         else
    //         {
    //             t2 = channel[(1024 - tCell) % 1024];
    //             dt = t1 - t2;
    //             for_each(channel.begin(), channel.end(), [&](float &val)
    //                      { val += dt; });
    //         }
    //     }
    // }
}

DAQFile &DAQFile::Initialise(DAQEvent &event)
{
    DAQFile &file = *this;

    if (!in_.is_open())
    {
        cerr << "!! Error: file not open --> use DAQFile(filename)" << endl;
        return file;
    }

    if (in_.tellg() != 0)
    {
        cerr << "!! Error: file already initialized --> ???" << endl;
        return file;
    }

    TAG bTag, cTag;
    vector<float> times(SAMPLES_PER_WAVEFORM);

    cout << "Initializing file " << filename_ << endl;

    file >> bTag; // DRSx
    file >> cTag; // TIME
    cout << bTag;

    if (bTag.tag[0] != 'D' and bTag.tag[1] != 'R' and bTag.tag[2] != 'S')
    {
        cerr << "!! Error: invalid file header --> expected \"DRS\", found " << bTag << endl;
        cerr << "Initialisation failed" << endl;
        return file;
    }
    if (strcmp(cTag.tag, "TIME") != 0)
    {
        cerr << "!! Error: invalid time header --> expected \"TIME\", found " << cTag << endl;
        cerr << "Initialisation failed" << endl;
        return file;
    }

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
    int i = 0, j = 0;
    while (file >> bTag) // B#?
    {
        cout << bTag << ":" << endl; // print --> B#?
        j = 0;
        while (file >> cTag) // C001
        {
            cout << " --> " << cTag << endl; // print --> C001
            file.Read(times);
            times_[i][j] = times;
            ++j;
        }
        ++i;
        file.ResetTag();
    }
    file.ResetTag();
    return file;
}

void DAQFile::Read(TAG &t)
{
    in_.read(t.tag, 4);
    n_ = t.tag[0];
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

void DAQFile::Read(vector<float> &vec, const unsigned short &range_center)
{
    unsigned short val;
    for (int i = 0; i < vec.size(); ++i)
    {
        in_.read((char *)&val, sizeof(unsigned short));
        vec[i] = val / 65536. + range_center / 1000. - 0.5;
    }
    return;
}

DAQFile &DAQFile::Close()
{
    if (in_.is_open())
    {
        cout << "Closing file " << filename_ << "..." << endl;
        in_.close();
    }
    else
    {
        cout << "File is already closed" << endl;
    }
    return *this;
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
    vector<float> volts(SAMPLES_PER_WAVEFORM);
    int i = 0, j = 0;

    event.is_ped_ = false;

    // Read only one event
    file >> eh;
    if (eh.serialNumber % 100 == 0 and eh.serialNumber > 0)
    {
        cout << "Event serial number: " << eh.serialNumber << endl;
    }

    while (file >> bTag)
    {
        file >> tag; // Trigger cell
        auto tCell = *(unsigned short *)(tag.tag + 2);
        j = 0;
        while (file >> cTag)
        {
            file >> tag; // Time scaler
            file.Read(volts, eh.rangeCenter);
            event.volts_[i][j] = volts;
            event.TimeCalibration(tCell, times_[i][j], i, j);
            ++j;
        }
        ++i;
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
    vector<float> volts(SAMPLES_PER_WAVEFORM);
    int i = 0, j = 0;

    event.is_ped_ = false;

    // Read only one event
    file >> eh;
    if (eh.serialNumber % 100 == 0)
    {
        cout << "Event serial number: " << eh.serialNumber << endl;
    }

    while (file >> bTag)
    {
        j = 0;
        while (file >> cTag)
        {
            file >> tag; // Time scaler
            file >> tag; // Trigger cell
            auto tCell = *(unsigned short *)(tag.tag + 2);
            file.Read(volts, eh.rangeCenter);
            event.volts_[i][j] = volts;
            event.TimeCalibration(tCell, times_[i][j], i, j);
            ++j;
        }
        file.ResetTag();
        ++i;
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
