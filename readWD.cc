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

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ FUNCTIONS                                                               │
  └─────────────────────────────────────────────────────────────────────────┘
 */

/*!
 @brief Function to print a @ref TAG istance easily on stream::cout

 @param o
 @param tag
 @return ostream&
 */
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

/*!
 @brief Function to print a @ref EventHeader easily on stream::cout

 @param o
 @param eh
 @return ostream&
 */
ostream &operator<<(ostream &o, const EventHeader &eh) // cout << EventHeader
{
    o << eh.tag << endl;
    o << "Date: " << eh.year << "/" << eh.month << "/" << eh.day << endl;
    o << "Hour: " << eh.hour << ":" << eh.min << ":" << eh.sec << "." << eh.ms << endl;
    o << "Range: " << eh.rangeCenter << " Serial number: " << eh.serialNumber;
    return o;
}

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ CLASSES : DAQEvent                                                      │
  └─────────────────────────────────────────────────────────────────────────┘
 */

/*!
 @brief Construct a new @ref DAQEvent() object.

 @details The constructor of the class has to initialize the correct values of some check-flags and of some variables
 that will be later evaluated.

 The default value for @ref DAQEvent::ped_interval_ is `{0, 100}` so that if the method
 @ref DAQEvent::SetPedInterval() is not called, the pedestal will be evaluated on the first 100 bins.

 The default value for @ref DAQEvent::iw_ is `{0, SAMPLES_PER_WAVEFORM - 1}`. When a method like @ref DAQEvent::GetCharge() is called,
 it calls also DAQEvent::EvalIntegrationBounds() and the new integration window is evaluated. Otherwise, the integration window can be setted
 by the user calling the method @ref DAQEvent::SetIntWindow().

 */
DAQEvent::DAQEvent()
{
    is_getch_ = false;
    is_init_ = false;
    is_iw_ = false;
    is_ped_ = false;
    is_peak_ = false;
    user_iw_ = false;
    ped_interval_ = {0, 100};
    iw_ = {0, SAMPLES_PER_WAVEFORM - 1};
    peak_threshold_ = 1.;
}

/*!
 @brief Select the channel to analyze.

 @details To select a channel, the user must consider only the channels that are turned on on the board. This means that the right order of the channel is
 not given by the channel ID number: the channel 6 on a WDB is not always the sixth channel, but it depends by the channels that are turned on. In any case,
 during the call to @ref DAQEvent::Initialise() a list of the active channel is displayed on terminal to double check.

 After the call to this method, the values of DAQEvent::ch_ are updated.

 @param board the index of the board, starting from 0.
 @param channel the index of the channel in the selected board, starting from 0.
 @return DAQEvent& to make cascade methods available.
 */
DAQEvent &DAQEvent::GetChannel(const int &board, const int &channel)
{
    if (board < 0 or channel < 0)
    {
        cerr << "!! Error: board and channel ID number(s) must be positive integers" << endl;
        exit(0);
    }

    else if (ch_.first == board and ch_.second == channel)
    {
        is_getch_ = true;
        return *this;
    }

    else if (volts_.find(board) != volts_.end())
    {
        if (volts_[board].find(channel) != volts_[board].end())
        {
            is_getch_ = true;
            ch_ = {board, channel};
            return *this;
        }
        cerr << "!! Error: invalid channel, max channel ID number for this board is " << volts_[board].size() - 1 << endl;
        exit(0);
    }
    cerr << "!! Error: invalid board, max board ID number is " << volts_.size() - 1 << endl;
    exit(0);
}

/*!
 @brief Function to set the interval where to perform pedestal evaluation.

 @details The function checks automatically which value in input is smaller to correctly order the pair passed. A first control is made to check
 if the inputs are in the correct boundary that is from 0 to @ref SAMPLES_PER_WAVEFORM.

 @param a The first boundary index value.
 @param b The second boundary index value.
 @return DAQEvent& to make cascade methods available.
 */
DAQEvent &DAQEvent::SetPedInterval(int a, int b)
{
    if (a > SAMPLES_PER_WAVEFORM or b > SAMPLES_PER_WAVEFORM)
    {
        cerr << "!! Error: in DAQEvent::SetPedInterval(), bound(s) must be lower of " << SAMPLES_PER_WAVEFORM << endl;
        cerr << "Bounds passed: a = " << a << ", b = " << b << endl;
        exit(0);
    }
    if (a < b)
    {
        ped_interval_ = {a, b};
    }
    else if (b < a)
    {
        ped_interval_ = {b, a};
    }
    else
    {
        cerr << "!! Error: invalid pedestal interval declared, values must be positive integers less than 1024" << endl;
        exit(0);
    }
    return *this;
}

/*!
 @brief Set the threshold in volt by the user.

 @details This threshold level is used by the code when the minima of the waveform are requested.

 @param thr The threshold level in Volts in a range (-0.5, +0.5)V
 @return DAQEvent&
 */
DAQEvent &DAQEvent::SetPeakThr(float thr)
{
    if ((thr < -0.5 or thr > 0.5) and thr != 1.)
    {
        cerr << "!! Error: invalid threshold level passed to function, it must be in the interval (-0.5, +0.5)V" << endl;
        cerr << "Threshold level passed: thr = " << thr << endl;
        exit(0);
    }
    peak_threshold_ = thr;
    return *this;
}

/*!
 @brief Set the integration window passing two indices that go from 0 to @ref SAMPLES_PER_WAVEFORM.

 @param a The left bound
 @param b The right bound
 @return DAQEvent&
 */
DAQEvent &DAQEvent::SetIntWindow(int a, int b)
{
    auto good_a = a < b and a >= 0;
    auto good_b = b < SAMPLES_PER_WAVEFORM;
    if (good_a and good_b)
    {
        iw_ = {a, b};
        user_iw_ = true;
    }
    else
    {
        cerr << "!! Error: invalid values passed as integration window" << endl;
        exit(0);
    }
    return *this;
}

/*!
 @brief Set the integration window passing time values in seconds.

 @details This method with this signature must be used after the call of DAQEvent::GetChannel().

 @param a The left bound
 @param b The right bound
 @return DAQEvent&
 */
DAQEvent &DAQEvent::SetIntWindow(float a, float b)
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel using DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    auto &times = times_[ch_.first][ch_.second];
    if (a < times[0] or a > b or b > times[SAMPLES_PER_WAVEFORM - 1])
    {
        cerr << "!! Error: invalid times passed as integration window" << endl;
        exit(0);
    }

    iw_.first = distance(times.begin(), lower_bound(times.begin(), times.end(), a));
    iw_.second = distance(times.begin(), lower_bound(times.begin() + iw_.first, times.end(), b));
    user_iw_ = true;
    return *this;
}

/*!
 @brief Method to evaluate charge in the integration region.

 @details The methods calls in order @ref DAQEvent::EvalPedestal() and @ref DAQEvent::EvalIntegrationBounds(). The pedestal
 is then subtracted from the waveform, the integration window is used for an integration over a correct window of values.

 @return float
 */
float DAQEvent::GetCharge()
{
    (*this).EvalPedestal();
    (*this).FindPeaks();
    (*this).EvalIntegrationBounds();

    const auto &volts = volts_[ch_.first][ch_.second];
    const auto &times = times_[ch_.first][ch_.second];
    int distance = iw_.second - iw_.first;

    is_getch_ = false;
    float charge = 0;

    for (int i = iw_.first; i < iw_.second; ++i)
    {
        charge += (volts[i + 1] + volts[i] - 2 * ped_.first) / (2 * (times[i + 1] - times[i]));
    }

    return abs(charge);
    return abs(accumulate(volts.begin() + iw_.first, volts.begin() + iw_.second, -distance * ped_.first));
}

/*!
 @brief Method to evaluate amplitude in the integration region.

 @return float
 */
float DAQEvent::GetAmplitude()
{
    (*this).EvalPedestal();
    (*this).FindPeaks();

    is_getch_ = false;

    return peak_.first - ped_.first;
}

/*!
 @brief Find the time at which the waveform goes under a given threshold level

 @param thr The threshold level passed by the user. It must be in a range from -0.5 to 0.5 V.
 @return The time requested
 */
float DAQEvent::GetTime(float thr)
{
    auto &volts = volts_[ch_.first][ch_.second];
    auto &times = times_[ch_.first][ch_.second];
    int i = 0;

    while (volts[i] > thr)
    {
        ++i;
    }

    if (i == SAMPLES_PER_WAVEFORM)
    {
        cout << "Time not found given threshold " << thr << ". Returning 0" << endl;
        return 0;
    }

    auto time = times[i] + (thr - volts[i]) * (times[i + 1] - times[i]) / (volts[i + 1] - volts[i]);

    // is_getch_ = false;

    return time;
}

/*!
 @brief Find the time at which the waveform goes under a given percentage of the waveform peak value

 @param CF the constant fraction value. Must be in range (0,1)
 @return The time requested
 */
float DAQEvent::GetTimeCF(float CF)
{
    if (CF <= 0 or CF > 1)
    {
        cerr << "!! Error: CF value must be in range (0, 1)" << endl;
        exit(0);
    }

    (*this).EvalPedestal();
    (*this).FindPeaks();
    (*this).EvalIntegrationBounds();

    float thr = ped_.first + (peak_.first - ped_.first) * CF;

    return (*this).GetTime(thr);
}

/*!
 @brief Evaluate the risetime of the waveform

 @return float
 */
float DAQEvent::GetRiseTime()
{
    return (*this).GetTimeCF(0.9) - (*this).GetTimeCF(0.1);
}

/*!
 @brief Getter method read-only for the attribute @ref DAQEvent::ped_.

 @return const pair<float, float>&
 */
const pair<float, float> &DAQEvent::GetPedestal()
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel with DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    (*this).EvalPedestal();
    is_getch_ = false;
    return ped_;
}

/*!
 @brief Getter method read-only for the attribute @ref DAQEvent::iw_.

 @return const pair<int, int>&
 */
const pair<int, int> &DAQEvent::GetIntegrationBounds()
{
    (*this).EvalPedestal();
    (*this).FindPeaks();
    (*this).EvalIntegrationBounds();
    is_getch_ = false;
    return iw_;
}

/*!
 @brief Getter method read-only for the waveform's voltages selected.

 @details The method checks if @ref DAQEvent::GetChannel() has been called. If it is the case, it returns
 the waveform's voltages of the selected board/channel.

 @return const vector<float>&
 */
const vector<float> &DAQEvent::GetVolts()
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel with DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    is_getch_ = false;
    return volts_[ch_.first][ch_.second];
}

/*!
 @brief Getter method read-only for the waveform's times selected.

 @details The method checks if @ref DAQEvent::GetChannel() has been called. If it is the case, it returns
 the waveform's times of the selected board/channel.

 @return const vector<float>&
 */
const vector<float> &DAQEvent::GetTimes()
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel with DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    is_getch_ = false;
    return times_[ch_.first][ch_.second];
}

/*!
 @brief

 @return const vector<int>&
 */
const vector<int> &DAQEvent::GetPeakIndices()
{
    (*this).EvalPedestal();
    (*this).FindPeaks();

    is_getch_ = false;

    return indexMin_;
}

/*!
     @brief Function to perform the time calibration.

     @details The time calibration is performed as specified in the DRS manual. The operation is done as following:
     \f[
        t_{ch}[i] = \sum_{j = 0}^{i - 1} dt_{ch}[(j + tCell)%1024]
     \f]

     @param tCell Cell number at which the signal triggered the board. Found in the event header.
     @param times Array with time bin width.
     @param i The index of the board.
     @param j The index of the channel.
 */
DAQEvent &DAQEvent::TimeCalibration(const unsigned short &tCell, const std::vector<float> &times, int i, int j)
{
    times_[i][j] = times;
    vector<float> &times_ij = times_[i][j];
    rotate(times_ij.begin(), times_ij.begin() + tCell, times_ij.end());
    partial_sum(times_ij.begin(), times_ij.end(), times_ij.begin());

    return *this;
}

/*!
 @brief Method to evaluate the pedestal of the currently selected waveform.

 @return DAQEvent&
 */
DAQEvent &DAQEvent::EvalPedestal()
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel using DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    int ped_interval_dist = ped_interval_.second - ped_interval_.first;
    const vector<float> &volts = volts_[ch_.first][ch_.second];
    ped_ = {0., 0.};
    ped_.first = accumulate(volts.begin() + ped_interval_.first, volts.begin() + ped_interval_.second, 0.) / ped_interval_dist;
    for (int i = ped_interval_.first; i < ped_interval_.second; ++i)
    {
        ped_.second += pow(volts[i] - ped_.first, 2);
    }
    ped_.second = sqrt(ped_.second / ped_interval_dist);

    return *this;
}

/*!
 @brief Method to evaluate the integration window of the currently selected waveform.

 @return DAQEvent&
 */
DAQEvent &DAQEvent::EvalIntegrationBounds()
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel using DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    if (user_iw_)
    {
        return *this;
    }

    iw_ = {indexMin_[0], indexMin_[0]};

    const vector<float> &volts = volts_[ch_.first][ch_.second];
    auto lower_bound = ped_.first - 5 * ped_.second;

    if (peak_.first < lower_bound)
    {
        while (volts[iw_.first] < lower_bound and iw_.first > 0)
        {
            --iw_.first;
        }

        while (volts[iw_.second] < lower_bound and iw_.second < SAMPLES_PER_WAVEFORM - 1)
        {
            ++iw_.second;
        }
    }

    return *this;
}

/*!
 @brief Method to find peaks in the integration window.

 @return DAQEvent&
 */
DAQEvent &DAQEvent::FindPeaks()
{
    if (!is_getch_)
    {
        cerr << "!! Error: select a channel using DAQEvent::GetChannel()" << endl;
        exit(0);
    }

    auto &volts = volts_[ch_.first][ch_.second];
    auto &times = times_[ch_.first][ch_.second];
    indexMin_ = {};

    if (user_iw_) // IW set by the user
    {
        auto index_min = distance(volts.begin(), min_element(volts.begin() + iw_.first, volts.begin() + iw_.second));
        indexMin_.push_back(index_min);
    }
    else if (peak_threshold_ == 1.) // No threshold level set by the user
    {
        auto index_min = distance(volts.begin(), min_element(volts.begin(), volts.end()));
        for (int i = 1; i < SAMPLES_PER_WAVEFORM - 1; ++i)
        {
            auto signal = volts[i] - ped_.first < -5 * ped_.second;
            auto min_left = abs(volts[i]) > abs(volts[i - 1]) + ped_.second;
            auto min_right = abs(volts[i]) > abs(volts[i + 1]) + ped_.second;
            auto at_least = volts[i] < volts[index_min] * 0.5;
            if (signal and min_left and min_right and at_least)
            {
                indexMin_.push_back(i);
            }
        }

        if (find(indexMin_.begin(), indexMin_.end(), index_min) == indexMin_.end()) // Sorted insertion of min element's index
        {
            auto pos = find_if(indexMin_.begin(), indexMin_.end(), [index_min](auto i)
                               { return i > index_min; });
            indexMin_.insert(pos, index_min);
        }
    }
    else // Threshold level set by the user
    {
        for (int i = 1; i < SAMPLES_PER_WAVEFORM - 1; ++i)
        {
            auto signal = volts[i] - ped_.first < -5 * ped_.second;
            auto min_left = abs(volts[i]) > abs(volts[i - 1]) + ped_.second;
            auto min_right = abs(volts[i]) > abs(volts[i + 1]) + ped_.second;
            auto at_least = volts[i] < peak_threshold_;
            if (signal and (min_left or min_right) and at_least)
            {
                indexMin_.push_back(i);
            }
        }
    }

    if (indexMin_.size() == 0)
    {
        auto index_min = distance(volts.begin(), min_element(volts.begin(), volts.end()));
        indexMin_.push_back(index_min);
    }

    peak_ = {volts[indexMin_[0]], times[indexMin_[0]]};

    return *this;
}

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ CLASSES : DAQConfig                                                     │
  └─────────────────────────────────────────────────────────────────────────┘
 */

DAQConfig::DAQConfig()
{
    is_makeconfig_ = false;
}

/*!
 @brief Initialise the configuration class with the default values for any board and any channel. 
 
 @param file 
 */
void DAQConfig::MakeConfig(DAQFile &file)
{
    if (file.initialization_ == false)
    {
        cerr << "!! Error : File was not initialised, use DAQFile::Open()" << endl;
        exit(0);
    }

    is_makeconfig_ = true;
    for (auto &[bKey, bVal] : file.times_)
    {
        for (auto &[cKey, cVal] : bVal)
        {
            intWindow_[bKey][cKey] = {0, SAMPLES_PER_WAVEFORM - 1};
            pedInterval_[bKey][cKey] = {0, 100};
            peakThr_[bKey][cKey] = +0.5;
        }
    }
}

void DAQConfig::SetIntWindow(pair<int, int> &intWindow, int b, int c)
{
    if (is_makeconfig_ == false)
    {
        cerr << "!! Error : Configuration class not initialised, use DAQEvent::MakeConfig()" << endl;
        exit(0);
    }

    if (intWindow.first < 0 || intWindow.first > intWindow.second || intWindow.second > SAMPLES_PER_WAVEFORM - 1)
    {
        cerr << "!! Error : Integration window has invalid value" << endl
             << " Values must be in interval (0, " << SAMPLES_PER_WAVEFORM << "), passed values are ( "<< intWindow.first << ", "<< intWindow.second << ")" << endl;
        exit(0); 
    } 

    if (intWindow_.find(b) != intWindow_.end())
    {
        if (intWindow_[b].find(c) != intWindow_[b].end())
        {
            intWindow_[b][c] = intWindow;
            return;
        }
    }
    cerr << "!! Error : Couldn't find board-channel of ID (" << b << ", " << c << ")" << endl;;
    exit(0);
}

void DAQConfig::SetIntWindow(pair<int, int> &intWindow)
{
    if (is_makeconfig_ == false)
    {
        cerr << "!! Error : Configuration class not initialised, use DAQEvent::MakeConfig()" << endl;
        exit(0);
    }

    if (intWindow.first < 0 || intWindow.first > intWindow.second || intWindow.second > SAMPLES_PER_WAVEFORM - 1)
    {
        cerr << "!! Error : Integration window has invalid value" << endl
             << " Values must be in interval (0, " << SAMPLES_PER_WAVEFORM << "), passed values are ( "<< intWindow.first << ", "<< intWindow.second << ")" << endl;
        exit(0); 
    } 

    for (auto &[bKey, bVal] : intWindow_)
    {
        for (auto &[cKey, cVal] : bVal)
        {
            cVal = intWindow;
        }
    }
}

void DAQConfig::SetPedInterval(pair<int, int> &pedInterval, int b, int c)
{
    if (is_makeconfig_ == false)
    {
        cerr << "!! Error : Configuration class not initialised, use DAQEvent::MakeConfig()" << endl;
        exit(0);
    }

    if (pedInterval.first < 0 || pedInterval.first > pedInterval.second || pedInterval.second > SAMPLES_PER_WAVEFORM - 1)
    {
        cerr << "!! Error : Pedestal interval has invalid value" << endl
             << " Values must be in interval (0, " << SAMPLES_PER_WAVEFORM << "), passed values are ( "<< pedInterval.first << ", "<< pedInterval.second << ")" << endl;
        exit(0); 
    } 

    if (pedInterval_.find(b) != pedInterval_.end())
    {
        if (pedInterval_[b].find(c) != pedInterval_[b].end())
        {
            pedInterval_[b][c] = pedInterval;
            return;
        }
    }
    cerr << "!! Error : Couldn't find board-channel of ID (" << b  << ", " << c << ")" << endl;
    exit(0);
}

void DAQConfig::SetPedInterval(pair<int, int> &pedInterval)
{
    if (is_makeconfig_ == false)
    {
        cerr << "!! Error : Configuration class not initialised, use DAQEvent::MakeConfig()" << endl;
        exit(0);
    }

    if (pedInterval.first < 0 || pedInterval.first > pedInterval.second || pedInterval.second > SAMPLES_PER_WAVEFORM - 1)
    {
        cerr << "!! Error : Pedestal interval has invalid value" << endl
             << " Values must be in interval (0, " << SAMPLES_PER_WAVEFORM << "), passed values are ( "<< pedInterval.first << ", "<< pedInterval.second << ")";
        exit(0); 
    } 

    for (auto &[bKey, bVal] : pedInterval_)
    {
        for (auto &[cKey, cVal] : bVal)
        {
            cVal = pedInterval;
        }
    }
}

void DAQConfig::SetPeakThr(float thr, int b, int c)
{
    if (is_makeconfig_ == false)
    {
        cerr << "!! Error : Configuration class not initialised, use DAQEvent::MakeConfig()" << endl;
        exit(0);
    }

    if (thr < -0.5 || thr > +0.5)
    {
        cerr << "!! Error : Peak threshold has invalid value" << endl
             << " Values must be in interval (-0.5, +0.5), passed values is " << thr << endl;
        exit(0); 
    } 

    if (peakThr_.find(b) != peakThr_.end())
    {
        if (peakThr_[b].find(c) != peakThr_[b].end())
        {
            peakThr_[b][c] = thr;
            return;
        }
    }
    cerr << "!! Error : Couldn't find board-channel of ID (" << b  << ", " << c << ")" << endl;
    exit(0);
}

void DAQConfig::SetPeakThr(float thr)
{
    if (is_makeconfig_ == false)
    {
        cerr << "!! Error : Configuration class not initialised, use DAQEvent::MakeConfig()" << endl;
        exit(0);
    }

    if (thr < -0.5 || thr > +0.5)
    {
        cerr << "!! Error : Peak threshold has invalid value" << endl
             << " Values must be in interval (-0.5, +0.5), passed values is " << thr << endl;
        exit(0); 
    }

    for (auto &[bKey, bVal] : peakThr_)
    {
        for (auto &[cKey, cVal] : bVal)
        {
            cVal = thr;
        }
    }
}

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ CLASSES : DAQFile                                                       │
  └─────────────────────────────────────────────────────────────────────────┘
 */

/*!
 @brief Construct a new DAQFile::DAQFile object

 */
DAQFile::DAQFile()
{
    std::cout << "Created DAQFile, open a file using DAQFile::Open()" << endl;
    is_lab_ = 0;
}

/*!
 @brief Construct a new DAQFile::DAQFile object.

 @param fname The file name to be opened.
 */
DAQFile::DAQFile(const string &fname)
{
    filename_ = fname;
    in_.open(fname, std::ios::in | std::ios::binary);
    std::cout << "Created DAQFile, opened file " << fname << std::endl;
    is_lab_ = 0;
    (*this).Initialise();
}

/*!
 @brief Initialise the file reading the **TIME** block.

 @return DAQFile&
 */
DAQFile &DAQFile::Initialise()
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

    file >> bTag; // DRSx (TIME for Lab's DRS boards)
    if (bTag.tag[0] == 'D' && bTag.tag[1] == 'R' && bTag.tag[2] == 'S')
    {
        cout << bTag;
        if (bTag.tag[3] == '8')
        {
            cout << " --> WaveDREAM Board" << endl;
        }
        else
        {
            cout << " --> DRS Evaluation Board" << endl;
        }
        file >> bTag; // TIME
    }
    else if (strcmp(bTag.tag, "TIME") == 0)
    {
        cout << "LAB-DRS" << endl;
        is_lab_ = 1;
    }
    else
    {
        cerr << "!! Error: invalid file header --> expected \"DRS\", found " << bTag << endl;
        cerr << "Initialisation failed" << endl;
        return file;
    }
    if (strcmp(bTag.tag, "TIME") != 0)
    {
        cerr << "!! Error: invalid time header --> expected \"TIME\", found " << cTag << endl;
        cerr << "Initialisation failed" << endl;
        return file;
    }

    o_ = 'B';
    initialization_ = 0;
    int i = 0, j = 0;
    while (file >> bTag)
    {
        cout << bTag << ":" << endl;
        j = 0;
        while (file >> cTag)
        {
            cout << " --> " << cTag << endl;
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

/*!
 @brief Method to close the file.

 @return DAQFile&
 */
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
 @brief Method to open a file given the file path and name.

 @param fname
 @return DAQFile&
 */
DAQFile &DAQFile::Open(const string &fname)
{
    if (!in_.is_open())
    {
        filename_ = fname;
        in_.open(fname, std::ios::in | std::ios::binary);
        std::cout << std::endl
                  << "Created DAQFile, opened file " << fname << std::endl;
        (*this).Initialise();
        return *this;
    }
    else
    {
        std::cerr << "!! Error: File is already opened --> " << filename_ << std::endl;
        return *this;
    }
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
    vector<float> volts(SAMPLES_PER_WAVEFORM);
    int i = 0, j = 0;

    // Read only one event
    file >> event.eh_;
    if (event.eh_.serialNumber % 100 == 0)
    {
        cout << "Event serial number: " << event.eh_.serialNumber << endl;
    }

    while (file >> bTag)
    {
        file >> tag; // Trigger cell
        auto tCell = *(unsigned short *)(tag.tag + 2);
        j = 0;
        while (file >> cTag)
        {
            if (!is_lab_)
            {
                file >> tag; // Time scaler, LAB-DRS don't have time scaler
            }
            file.Read(volts, event.eh_.rangeCenter);
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
    vector<float> volts(SAMPLES_PER_WAVEFORM);
    int i = 0, j = 0;

    // Read only one event
    file >> event.eh_;
    if (event.eh_.serialNumber % 100 == 0 and event.eh_.serialNumber > 0)
    {
        cout << "Event serial number: " << event.eh_.serialNumber << endl;
    }

    while (file >> bTag)
    {
        j = 0;
        while (file >> cTag)
        {
            file >> tag; // Time scaler
            file >> tag; // Trigger cell
            auto tCell = *(unsigned short *)(tag.tag + 2);
            file.Read(volts, event.eh_.rangeCenter);
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

/*!
 @brief Read a tag.

 @param t
 */
void DAQFile::Read(TAG &t)
{
    in_.read(t.tag, 4);
    n_ = t.tag[0];
    return;
}

/*!
 @brief Read an event header.

 @param eh
 */
void DAQFile::Read(EventHeader &eh)
{
    in_.read((char *)&eh, sizeof(eh));
    n_ = eh.tag[0];
}

/*!
 @brief Read a vector of float.

 @param vec
 */
void DAQFile::Read(vector<float> &vec)
{
    for (int i = 0; i < vec.size(); ++i)
    {
        in_.read((char *)&vec.at(i), sizeof(float));
    }
    return;
}

/*!
 @brief Read a vector of voltages.

 @details The reading of a vector of voltages implies also the trasformation from integer to Volts, and the shift due to the range center.

 @param vec
 @param range_center
 */
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

/*!
 @brief The evaluation of boolean for the class DAQFile.

 @details This method evaluates all the valid possible combinations of consecutive tags to determine if the value to return is 1 or 0.
 It uses the same principle of a finite state machine with the usage of a map named **header**, where the combinations of key-value match the
 possible configurations of consecutive non-equal tags that can be found in the binary file.

 @return true
 @return false
 */
DAQFile::operator bool()
{
    map<char, char> header{{'E', 'B'}, {'B', 'C'}, {'C', 'B'}};
    if (!in_.good())
    {
        cout << "End of file reached" << endl;
        return 0;
    }
    else if (n_ == 'T' or n_ == 'D') // Ignores DRSx and TIME
        return 0;
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
