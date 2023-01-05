#ifndef READWD_H
#define READWD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>
#include <numeric>

#define SAMPLES_PER_WAVEFORM 1024

// ----- CLASSES -----

struct TAG
{
    TAG() { tag[4] = '\0'; }
    char tag[5];
};

/*!
 @brief Struct to contain the event header informations.

 @details The event header is written as follows on .dat files deriving from the DRS/WDB

 Inserire tabella... aaa

 */
struct EventHeader
{
    char tag[4];
    unsigned int serialNumber;
    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short min;
    unsigned short sec;
    unsigned short ms;
    unsigned short rangeCenter;
};

/*!
 @brief Another class

 */
class DAQEvent
{
    using MAP = std::map<std::string, std::map<std::string, std::vector<float>>>;

public:
    std::vector<float> GetChannel(const int &);
    std::vector<float> GetChannel(const int &, const int &);
    std::vector<float> GetChannel(const std::string &, const std::string &);

    float GetCharge();
    float GetAmplitude();
    float GetPedestal();

    MAP GetTimeMap() const { return times_; };
    MAP GetVoltMap() const { return volts_; };

private:
    void TimeCalibration(const unsigned short &);
    void CreateBoard(const TAG &);
    void CreateChannel(const TAG &, const TAG &);
    void SetTrigger(const unsigned short &);
    void SetTime(const std::vector<float> &);
    void SetVolts(const TAG &, const TAG &, const std::vector<float> &);

    unsigned short trigger_;
    MAP times_;
    MAP volts_;

    friend class DAQFile;
};

class DRSEvent : public DAQEvent
{
};

class WDBEvent : public DAQEvent
{
};

class DAQFile
{
public:
    DAQFile(const std::string &filename)
    {
        filename_ = filename;
        in_.open(filename, std::ios::in | std::ios::binary);
        std::cout << "Created DAQFile, opened file " << filename << std::endl;
    }
    ~DAQFile() { in_.close(); }

    void Initialise(DAQEvent &);
    void Close();

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

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif