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
#include <iterator>
#include <set>
#include <map>
#include <numeric>

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
    using MAP = std::map<std::string, std::map<std::string, std::vector<float>>>;

public:
    DAQEvent() { is_ped_ = false; }

    DAQEvent &GetChannel(const int &);
    DAQEvent &GetChannel(const int &, const int &);
    DAQEvent &GetChannel(const std::string &, const std::string &);

    float GetCharge();
    float GetAmplitude();
    std::pair<float, float> GetPedestal();

    MAP GetTimeMap() const { return times_; };
    MAP GetVoltMap() const { return volts_; };

private:
    void TimeCalibration(const unsigned short &);
    void CreateBoard(const TAG &);
    void CreateChannel(const TAG &, const TAG &);
    void SetTrigger(const unsigned short &);
    void SetTime(const std::vector<float> &);
    void SetVolts(const TAG &, const TAG &, const std::vector<float> &);

    void EvalPedestal();

    unsigned short trigger_;
    MAP times_; ///< Data structure to hold times' information.
    MAP volts_;
    std::set<std::string> set_getchannelS_;
    std::set<int> set_getchannelI_;
    std::set<std::string> set_getChannelII_;

    std::vector<float> wfVolts_;
    std::vector<float> wfTimes_;

    std::pair<float, float> ped_;

    bool is_ped_;

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
public:
    DAQFile(const std::string &fname)
    {
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

private:
    operator bool();
    template <class T>
    void Read(T &t) { in_.read((char *)&t, sizeof(t)); }
    void Read(TAG &t)
    {
        in_.read(t.tag, 4);
        n_ = t.tag[0];
    }
    void Read(EventHeader &);
    void Read(std::vector<float> &);
    void Read(std::vector<unsigned short> &);
    void ResetTag() { in_.seekg(-4, in_.cur); }

    std::string filename_;
    std::ifstream in_;
    char o_, n_;
    bool initialization_;
};

// ----- FUNCTIONS -----

float GetCharge(std::vector<float>);

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif