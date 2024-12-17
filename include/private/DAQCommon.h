#ifndef DAQCOMMON_H
#define DAQCOMMON_H

#include <array>
#include <map>

inline const int WAVEFORMSAMPLE = 1024; ///< Number of samples in a waveform

class TimeArray_t
{
	public:
		TimeArray_t() = default;
		TimeArray_t(const float &time);

		float operator[](const int &index) const { return _timeArray[index]; };
		float &operator[](const int &index) { return _timeArray[index]; };

		std::array<float, WAVEFORMSAMPLE> * GetTimeArray() { return &_timeArray; };

		void Calibrate(const unsigned int& tCell);

	private:
		std::array<float, WAVEFORMSAMPLE> _timeArray;
};

class VoltArray_t
{
	public:
		VoltArray_t() = default;
		VoltArray_t(const float &voltage);

		unsigned int operator[](const int &index) const { return _voltArray[index]; };
		unsigned int &operator[](const int &index) { return _voltArray[index]; };

		std::array<unsigned int, WAVEFORMSAMPLE> * GetVoltArray() { return &_voltArray; };

		void Calibrate(const unsigned int& range);

	private:
		std::array<unsigned int, WAVEFORMSAMPLE> _voltArray;
};

typedef std::pair<TimeArray_t, VoltArray_t> Channel_t;

typedef std::map<int, Channel_t> Board_t;

/*!
 * @brief Enum class for Board Type
 *
 * Enum class for Board Type
 *
 * @see DAQReader
 */
enum class BoardType_t : unsigned short
{
	WDB, ///< WaveDREAM Board
	DRS, ///< Domino Ring Sampler Board
	LAB	 ///< LAB-Variant of DRS Board
};

/*!
 * @brief Enum class for Block Type
 *
 * Enum class for Block Type
 *
 * @see DAQReader
 */
enum class BlockType_t : int
{
	F_HEADER, ///< File Header
	T_HEADER, ///< Time Header
	B_HEADER, ///< Board Header
	C_HEADER, ///< Channel Header
	E_HEADER, ///< Event Header
	T_ARRAY,  ///< Time Array
	V_ARRAY	  ///< Voltage Array
};

struct EventHeader_t
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

#endif // DAQCOMMON_H
