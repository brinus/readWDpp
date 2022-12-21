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

class DAQEvent
{
public:
    void TimeCalibration();
    DAQEvent GetChannel(const int &);
    float GetCharge();
    float GetAmplitude();
    float GetPedestal();
    void CreateBoard(const TAG &);
    void Time(float *);

private:
    unsigned int trigger_;
    float timeCal_[SAMPLES_PER_WAVEFORM];
    float voltages_[SAMPLES_PER_WAVEFORM];
    std::map<std::string, std::vector<float *>> times_;
    std::map<std::string, std::vector<float *>> volts_;
};

class DAQFile
{
public:
    DAQFile() { o_ = 'B'; };
    DAQFile(const std::string &filename)
    {
        in_.open(filename, std::ios::in | std::ios::binary);
        o_ = 'B';
    }
    ~DAQFile() { in_.close(); }

    void Read(TAG &t)
    {
        in_.read(t.tag, 4);
        n_ = t.tag[0];
    }
    template <class T>
    void Read(T &t) { in_.read((char *)&t, sizeof(t)); }
    void ResetTag() { in_.seekg(-4, in_.cur); }
    void Initialise(DAQEvent &);

    operator bool();

private:
    char o_, n_;
    std::ifstream in_;
};

class DAQFiles
{
    using strvec_t = std::vector<std::string>;
    using iterator = strvec_t::iterator;
    using const_iterator = strvec_t::const_iterator;

public:
    DAQFiles(){};
    DAQFiles(const strvec_t &filenames) : filenames_(filenames){};
    ~DAQFiles(){};

    iterator begin() { return filenames_.begin(); }
    iterator end() { return filenames_.end(); }
    const_iterator begin() const { return filenames_.begin(); }
    const_iterator end() const { return filenames_.end(); }
    const_iterator cbegin() const { return filenames_.cbegin(); }
    const_iterator cend() const { return filenames_.cend(); }

private:
    strvec_t filenames_;
};

// FUNCTIONS

DAQFile &operator>>(DAQFile &, TAG &);
DAQFile &operator>>(DAQFile &, EventHeader &);

std::ostream &operator<<(std::ostream &, const TAG &);
std::ostream &operator<<(std::ostream &, const EventHeader &);

#endif