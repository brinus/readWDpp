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
#include <algorithm>
#include <math.h>
#include <cstring>

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
    /*!
     @brief Construct a new TAG object

     The last char is set equal to `\0`
     */
    TAG() { tag[4] = '\0'; }
    char tag[5]; ///< The array of char
};

/*!
 @brief Informations about the event contained in the file.

 Everytime an @ref EventHeader is read, the infos about the event are read in order just as specified in the
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

class DAQConfig;
class DAQEvent;
class DAQFile;

/*!
 @brief Main class to hold various settings for the channels.

 This class offers a more customizable setup for the file to be read. For each event, for each board and channel, it is possible to set a different value of trigger threshold, integration
 window or pedestal interval.

 */
class DAQConfig
{
private:
    DAQConfig();

    void MakeConfig(DAQFile &);
    void ShowConfig();

    std::map<int, std::map<int, std::pair<int, int>>> intWindow_;   ///< Data member to hold integration windows intervals of various channels.
    std::map<int, std::map<int, std::pair<int, int>>> pedInterval_; ///< Data member to hold pedestal intervals of various channels.
    std::map<int, std::map<int, float>> peakThr_;                   ///< Data member to hold peak threshold values of various channels.

    void SetIntWindow(std::pair<int, int>, int, int);
    void SetIntWindow(std::pair<int, int>);
    void SetPedInterval(std::pair<int, int>, int, int);
    void SetPedInterval(std::pair<int, int>);
    void SetPeakThr(float, int, int);
    void SetPeakThr(float);

    bool is_makeconfig_; ///< Flag to check if the method @ref DAQConfig::MakeConfig() has been called at least once.

    friend class DAQEvent;
    friend class DAQFile;
};

/*!
 @brief Main class to store voltage and time values.

 This class is used with @ref DAQFile to read each event contained into a file.

 */
class DAQEvent
{
    using MAP = std::map<int, std::map<int, std::vector<float>>>; ///< Alias for data structure.

public:
    DAQEvent &GetChannel(const int &, const int &);
    void SetPedInterval(int, int);
    void SetPeakThr(float);
    void SetIntWindow(int, int);
    void SetIntWindow(float, float);

    /*!
     @brief Method to simply call @ref DAQConfig::MakeConfig().
     
     @param file The file to be read.
     */
    void MakeConfig(DAQFile &file) { config_.MakeConfig(file); };
    /*!
     @brief Simple method to call @ref DAQConfig::ShowConfig().
     
     */
    void ShowConfig() { config_.ShowConfig(); };

    float GetCharge();
    float GetAmplitude();
    float GetTime(float);
    float GetTimeCF(float);
    float GetRiseTime();
    const std::pair<float, float> &GetPedestal();
    const std::vector<float> &GetVolts();
    const std::vector<float> &GetTimes();
    const std::vector<int> &GetPeakIndices();
    const std::pair<int, int> &GetIntegrationBounds();
    const EventHeader &GetEH() { return eh_; };

    const MAP &GetVoltMap() { return volts_; };
    const MAP &GetTimeMap() { return times_; };

protected:
    DAQEvent();

private:
    DAQEvent &TimeCalibration(const unsigned short &, const std::vector<float> &, int, int);
    DAQEvent &EvalPedestal();
    DAQEvent &EvalIntegrationBounds();
    DAQEvent &FindPeaks();

    MAP times_; ///< Structure to hold integrated times values of all boards and channels.
    MAP volts_; ///< Structure to hold voltage values of all boards and channels.

    EventHeader eh_;
    DAQConfig config_; ///< Class to hold settings about pedestal and integration window intervals.

    std::pair<float, float> ped_;      ///< Pair to hold pedestal *mean* and pedestal *std.dev.*.
    std::pair<float, float> peak_;     ///< Pair to hold value of voltage and time at the peak.
    std::pair<int, int> ped_interval_; ///< Pair to hold indices as boundary edges where pedestal is evaluated.
    std::pair<int, int> iw_;           ///< Pair to hold indices as boundary edges where integration is performed by @ref DAQEvent::GetCharge().
    std::pair<int, int> ch_;           ///< Pair to hold indices of board and channel selected;
    std::vector<int> indexMin_;        ///< Indices of local minima found.

    bool is_init_;         ///< Flag to check if the file is initialised
    bool is_getch_;        ///< Flag to check if the method @ref DAQEvent::GetChannel() has been called
    bool is_iw_;           ///< Flag to check if integration window has been evaluated once
    bool is_ped_;          ///< Flag to check if pedestal has been evaluated once
    bool is_peak_;         ///< Flag to check if peaks have been found once
    bool user_iw_;         ///< Flag to check if the integration window was passed by the user
    float peak_threshold_; ///< Value to store the threshold in volts passed by the user

    friend class DAQFile;
};

/*!
 @brief Specialization of DAQEvent to a DRS board.

 */
class DRSEvent : public DAQEvent
{
};

/*!
 @brief Specialization of DAQEvent to a WaveDREAM board.

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

    DAQFile &Close();
    DAQFile &Open(const std::string &);

    bool operator>>(DRSEvent &);
    bool operator>>(WDBEvent &);

    const MAP &GetTimeMap() { return times_; };

private:
    DAQFile &Initialise();

    bool operator>>(TAG &);
    bool operator>>(EventHeader &);
    operator bool();
    void Read(TAG &);
    void Read(EventHeader &);
    void Read(std::vector<float> &);
    void Read(std::vector<float> &, const unsigned short &);
    void ResetTag() { in_.seekg(-4, in_.cur); }

    std::string filename_; ///< The name of the file
    std::ifstream in_;     ///< The input file to read
    char o_;               ///< The initial letter of the previous tag read
    char n_;               ///< The initial letter of the newest tag read
    bool initialization_;  ///< Flag to store if @ref DAQFile::Initialise() was already called
    MAP times_;            ///< Struct to hold \f$ \Delta t\f$ read from the ```TIME```part of the file
    bool is_lab_;          ///< Flag to check if the board is from LAB or not;

    friend class DAQConfig;
};

/*
  ┌─────────────────────────────────────────────────────────────────────────┐
  │ FUNCTIONS                                                               │
  └─────────────────────────────────────────────────────────────────────────┘
 */

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif