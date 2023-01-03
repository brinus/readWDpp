#ifndef READWD_H
#define READWD_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>

#define SAMPLES_PER_WAVEFORM 1024

// CLASSES

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
    DAQEvent GetChannel(const int &);

    float GetCharge();
    float GetAmplitude();
    float GetPedestal();

    // Dovrebbero essere protetti
    void CreateBoard(const TAG &);
    void CreateChannel(const TAG &, const TAG &);
    void SetTrigger(const unsigned short &);
    void SetTime(const std::vector<float> &);
    void SetVolts(const TAG &, const TAG &, const std::vector<float> &);
    void TimeCalibration();

    MAP GetTimeMap() const { return times_; };
    MAP GetVoltMap() const { return volts_; };

private:
    unsigned short trigger_;
    MAP times_;
    MAP volts_;
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

    void Initialise(DAQEvent &);
    void Close();

    operator bool();
    bool operator>>(DRSEvent &);
    bool operator>>(WDBEvent &);
    bool operator>>(TAG &);
    bool operator>>(EventHeader &);

private:
    void ResetTag() { in_.seekg(-4, in_.cur); }

    std::string filename_;
    std::ifstream in_;
    char o_, n_;
    bool initialization_;
};

class DAQFiles
{
    using strvec_t = std::vector<std::string>;
    using DAQvec_t = std::vector<DAQFile *>;
    using iterator = DAQvec_t::iterator;
    using const_iterator = DAQvec_t::const_iterator;

public:
    DAQFiles(){};
    DAQFiles(const strvec_t &filenames);
    ~DAQFiles(){};

    iterator begin() { return files_.begin(); }
    iterator end() { return files_.end(); }
    const_iterator begin() const { return files_.begin(); }
    const_iterator end() const { return files_.end(); }
    const_iterator cbegin() const { return files_.cbegin(); }
    const_iterator cend() const { return files_.cend(); }

private:
    strvec_t filenames_;
    DAQvec_t files_;
};

// FUNCTIONS

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif