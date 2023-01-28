/*!
 @file readWD.hh
 @author Matteo Brini (brinimatteo@gmail.com)
 @brief Declaration of classes and methods.
 @version 0.1
 @date 2023-01-05

 @copyright Copyright (c) 2023

 */

#ifndef READWD_H
#define READWD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <numeric>

#define SAMPLES_PER_WAVEFORM 1024 ///< The number of samples made by the waveforms, both DRS and WDB.

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ CLASSES                                                                 │
  └─────────────────────────────────────────────────────────────────────────┘
 */

/*!
 @brief 5 char long word ended with '\0' for simple printing.

 @details This struct is used to read the various headers in the file.
 */
struct TAG
{
    TAG() { tag[4] = '\0'; }
    char tag[5];
};

/*!
 @brief Informations about the event contained in the file.

 @details Everytime an @ref EventHeader is read, the infos about the event are read in order just as specified in the
          DRS Evaluation Board manual.
 */
struct EventHeader
{
    char tag[4];                ///< The tag of the event header.
    unsigned int serialNumber;  ///< The serial number of the event: starting from 1 for DRS and 0 for WDB.
    unsigned short year;        ///< The year.
    unsigned short month;       ///< The month.
    unsigned short day;         ///< The day.
    unsigned short hour;        ///< The hour.
    unsigned short min;         ///< The minute.
    unsigned short sec;         ///< The second.
    unsigned short ms;          ///< The millisecond.
    unsigned short rangeCenter; ///< The rangeCenter (in Volts).
};

/*!
 @brief Main class to store voltage and time values.

 @details This class is used with @ref DAQFile to read each event contained into a file.

 */
class DAQEvent
{
    using MAP = std::map<int, std::map<int, std::vector<float>>>; ///< Alias for data structure.

public:
    DAQEvent();

    DAQEvent &GetChannel(const int &, const int &);
    DAQEvent &SetPedInterval(int, int);
    DAQEvent &SetPeakThr(float);
    DAQEvent &SetIntWindow(int, int);
    DAQEvent &SetIntWindow(float, float);

    float GetCharge();
    float GetAmplitude();
    float FindTime(float);
    const std::pair<float, float> &GetPedestal();
    const std::pair<int, int> &GetIntegrationBounds();
    const std::vector<float> &GetVolts();
    const std::vector<float> &GetTimes();
    const std::vector<int> &GetPeakIndices();

    const MAP &GetVoltMap() { return volts_; };
    const MAP &GetTimeMap() { return times_; };

private:
    DAQEvent &TimeCalibration(const unsigned short &, const std::vector<float> &, int, int);
    DAQEvent &EvalPedestal();
    DAQEvent &EvalIntegrationBounds();
    DAQEvent &FindPeaks();

    MAP times_; ///< Structure to hold integrated times array of all board and channels.
    MAP volts_; ///< Structure to hold voltage values of all board and channels.

    std::pair<float, float> ped_;      ///< Pair to hold pedestal *mean* and pedestal *std.dev.*.
    std::pair<float, float> peak_;     ///< Pair to hold value of voltage and time at the peak.
    std::pair<int, int> ped_interval_; ///< Pair to hold indices as boundary edges where pedestal is evaluated.
    std::pair<int, int> iw_;           ///< Pair to hold indices as boundary edges where integration is performed by @ref DAQEvent::GetCharge().
    std::pair<int, int> ch_;           ///< Pair to hold indices of board and channel selected;
    std::vector<int> indexMin_;        ///< Indices of local minima found.

    bool is_init_;
    bool is_getch_;
    bool is_iw_;
    bool user_iw_;
    float peak_threshold_;

    friend class DAQFile;
};

/*!
 @brief

 */
class DRSEvent : public DAQEvent
{
};

/*!
 @brief

 */
class WDBEvent : public DAQEvent
{
};

/*!
 @brief Class to manage the file and the outputing of data in a @ref DAQEvent instance.

 */
class DAQFile
{
    using MAP = std::map<int, std::map<int, std::vector<float>>>; ///< Alias for data structure.

public:
    DAQFile();
    DAQFile(const std::string &);
    ~DAQFile() { in_.close(); }

    DAQFile &Initialise();
    DAQFile &Close();
    DAQFile &Open(const std::string &);

    bool operator>>(TAG &);
    bool operator>>(EventHeader &);
    bool operator>>(DRSEvent &);
    bool operator>>(WDBEvent &);

private:
    operator bool();
    void Read(TAG &);
    void Read(EventHeader &);
    void Read(std::vector<float> &);
    void Read(std::vector<float> &, const unsigned short &);
    void ResetTag() { in_.seekg(-4, in_.cur); }

    std::string filename_;
    std::ifstream in_;
    char o_, n_;
    bool initialization_;
    MAP times_;
};

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ FUNCTIONS                                                               │
  └─────────────────────────────────────────────────────────────────────────┘
 */

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif