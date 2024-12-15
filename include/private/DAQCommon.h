#ifndef DAQCOMMON_H
#define DAQCOMMON_H

#include <array>
#include <map>

inline const int WAVEFORMSAMPLE = 1024; ///< Number of samples in a waveform


typedef std::map<int, Channel_t> Board_t;
typedef std::pair<TimeArray_t, VoltArray_t> Channel_t;

class TimeArray_t
{
	public:
		TimeArray_t() = default;
		TimeArray_t(const float &time);

		float operator[](const int &index) const { return _timeArray[index]; };
		float &operator[](const int &index) { return _timeArray[index]; };

		void Calibrate(const unsigned int& tCell);

	private:
		std::array<float, WAVEFORMSAMPLE> _timeArray;
};

class VoltArray_t
{
	public:
		VoltageArray_t();
		VoltageArray_t(const float &voltage);

		unsigned int operator[](const int &index) const { return _voltageArray[index]; };
		unsigned int &operator[](const int &index) { return _voltageArray[index]; };

		void Calibrate(const unsigned int& range);

	private:
		std::array<unsigned int, WAVEFORMSAMPLE> _voltageArray;
};

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

#endif // DAQCOMMON_H
