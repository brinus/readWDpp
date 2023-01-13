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
#include <stdexcept>

#define SAMPLES_PER_WAVEFORM 1024 ///< The number of samples made by the waveforms, both DRS and WDB.

// ----- CLASSES -----

struct IntegrationWindow
{
    int start;
    int stop;
};

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
    unsigned short rangeCenter; /// The rangeCenter (in Volts).
};

/*!
 @brief Class to store time and voltage data.

 */
class DAQEvent
{
    using MAP = std::map<int, std::map<int, std::vector<float>>>;

public:
    DAQEvent() { is_ped_ = false; }

    DAQEvent &GetChannel(const int &, const int &);

    float GetCharge();
    float GetAmplitude();
    const std::pair<float, float> &GetPedestal();
    const std::vector<float> &GetVolts();
    const std::vector<float> &GetTimes();

    const MAP &GetTimeMap() { return times_; };
    const MAP &GetVoltMap() { return volts_; };

private:
    void TimeCalibration(const unsigned short &, const std::vector<float> &, int, int);
    void EvalPedestal();

    MAP times_;
    MAP volts_;

    std::vector<float> wfVolts_;
    std::vector<float> wfTimes_;
    std::pair<float, float> ped_;

    bool is_ped_;
    bool is_getch_;

    friend class DAQFile;
};

class DRSEvent : public DAQEvent
{
};

class WDBEvent : public DAQEvent
{
};

/*!
 @brief Class to manage the file and the outputing of data in a @ref DAQEvent instance.

 */
class DAQFile
{
    using MAP = std::map<int, std::map<int, std::vector<float>>>;

public:
    DAQFile(const std::string &fname)
    {
        TAG tag;
        filename_ = fname;
        in_.open(fname, std::ios::in | std::ios::binary);
        std::cout << "Created DAQFile, opened file " << fname << std::endl;
    }
    ~DAQFile() { in_.close(); }

    DAQFile &Initialise(DAQEvent &);
    DAQFile &Close();
    DAQFile &Open(const std::string &fname)
    {
        if (!in_.is_open())
        {
            filename_ = fname;
            in_.open(fname, std::ios::in | std::ios::binary);
            std::cout << std::endl
                      << "Created DAQFile, opened file " << fname << std::endl;
            return *this;
        }
        else
        {
            std::cerr << "!! Error: File is already opened --> " << filename_ << std::endl;
            return *this;
        }
    }
    bool operator>>(DRSEvent &);
    bool operator>>(WDBEvent &);
    bool operator>>(TAG &);
    bool operator>>(EventHeader &);
    const char &GetVersion() { return version_; }

private:
    operator bool();
    void Read(TAG &);
    void Read(EventHeader &);
    void Read(std::vector<float> &);
    void Read(std::vector<float> &, const unsigned short &);
    void ResetTag() { in_.seekg(-4, in_.cur); }

    char version_;
    std::string filename_;
    std::ifstream in_;
    char o_, n_;
    bool initialization_;
    MAP times_;
};

// ----- FUNCTIONS -----

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif