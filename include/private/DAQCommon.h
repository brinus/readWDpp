#ifndef DAQCOMMON_H
#define DAQCOMMON_H

inline const int WAVEFORMSAMPLE = 1024; ///< Number of samples in a waveform

/*!
 * @brief Enum class for Board Type
 * 
 * Enum class for Board Type
 * 
 * @see DAQReader
 */
enum class BoardType_t : unsigned short
{
	WDB,	///< WaveDREAM Board
	DRS,	///< Domino Ring Sampler Board
	LAB		///< LAB-Variant of DRS Board
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
	F_HEADER,	///< File Header
	T_HEADER,	///< Time Header
	B_HEADER,	///< Board Header
	C_HEADER,	///< Channel Header
	E_HEADER,	///< Event Header
	T_ARRAY,	///< Time Array
	V_ARRAY		///< Voltage Array
};

#endif // DAQCOMMON_H
