#ifndef PIXIESETTINGS_H
#define PIXIESETTINGS_H

// Defining the different sample rates
#define SAMPLE_100MHZ 100   ///< Sample rate of the 100 MHz model
#define SAMPLE_250MHZ 250   ///< Sample rate of the 250 MHz model
#define SAMPLE_500MHZ 500   ///< Sample rate of the 500 MHz model


// Definition of the different possible header sizes.
#define SHORT_HEADER    4  ///< Only time, CFD and energy + trace
#define MID_HEADER      8    ///< SHORT_HEADER + EnergySum
#define LONG_HEADER     12  ///< SHORT_HEADER + QDCSum
#define FULL_HEADER     16  ///< SHORT_HEADER + EnergySum + QDCSum

// Defining the offset of the start of the EnergySums and QDCSums in terms of 32 bit words.
#define ENERGYSUM_MID_OFFSET    4   ///< Offset in words for the EnergySums for mid header size
#define ENERGYSUM_FULL_OFFSET   4   ///< Offset in words for the EnergySums for full header size
#define QDCSUM_LONG_OFFSET      4   ///< Offset in words for the QDCSums for long header size
#define QDCSUM_FULL_OFFSET      8   ///< Offset in words for the QDCSums for full header size


// Definition of the EnergySum and QDCSum sizes.
#define ENERGYSUM_LENGTH    4   ///< Number of 32 bit values of EnergySum
#define QDCSUM_LENGTH       8   ///< Number of 32 bit values of QDCSum

// Definition of bitmasks for decoding the events.
#define FINISHCODE              0xC0000000  ///< Bitmask for the Finish code bit [31]
#define FINISHCODE_OFFSET       30          ///< Finish code offset
#define EVENTLENGTH             0x3FFE0000  ///< Bitmask for the Event length [30:17]
#define EVENTLENGTH_OFFSET      17          ///< Event length offset
#define HEADERSIZE              0x0001F000  ///< Header size [16:12]
#define HEADERSIZE_OFFSET       12          ///< Header size offset
#define CHANNELID               0x00000FFF  ///< Channel ID [11:0]
#define CHANNELID_OFFSET        0           ///< Channel ID offset
#define EVENTTIMELOW            0xFFFFFFFF  ///< Least significant bits of the timestamp [31:0]
#define EVENTTIMELOW_OFFSET     0           ///< Least siginificant bits of the timestamp offset
#define EVENTTIMEHIGH           0x0000FFFF  ///< Most significant bits of the timestamp [15:0]
#define EVENTTIMEHIGH_OFFSET    0           ///< Most significant bits of the timestamp offset

// Model dependent bitmaks

// 100 MHz model
#define CFDFAILBIT_100MHZ       0x80000000  ///< CFD fail bit [31]
#define CFDTRIGSOURCE_100MHZ    0x00000000  ///< CFD Trig. source (N/A for 100MHz)
#define CFDFRACTIONTIME_100MHZ  0x7FFF0000  ///< CFD fraction time [30:16]

// 250 MHz model
#define CFDFAILBIT_250MHZ       0x80000000  ///< CFD fail bit [31]
#define CFDTRIGSOURCE_250MHZ    0x40000000  ///< CFD Trig. source [30]
#define CFDFRACTIONTIME_250MHZ  0x3FFF0000  ///< CFD fraction time [29:16]

// 500 MHz model
#define CFDFAILBIT_500MHZ       0x00000000  ///< CFD fail bit (N/A for 500 MHz)
#define CFDTRIGSOURCE_500MHZ    0xC0000000  ///< CFD Trig. source [31:30]
#define CFDFRACTIONTIME_500MHZ  0x1FFF0000  ///< CFD fraction time [28:16]

// Offset is currently common
#define CFDFAILBIT_OFFSET       31
#define CFDTRIGSOURCE_OFFSET    30
#define CFDFRACTIONTIME_OFFSET  16


// Bitmask for trace data, each 32 bit word are packed with two adc values
#define FIRST_ADC           0x00000FFF              ///< First ADC value [11:0]
#define FIRST_ADC_OFFSET    0                       ///< Offset of first ADC value
#define SECOND_ADC  0x0FFF0000                      ///< Second ADC value [27:16]
#define SECOND_ADC_OFFSET   16                      ///< Offset of second ADC value

// Bitmask for the trace length and energy offset
#define TRACELENGTH         0xFFFF0000      ///< Trace length [31:16]
#define TRACELENGTH_OFFSET  16              ///< Trace length offset
#define ENERGY              0x0000FFFF      ///< Event energy [16:0]
#define ENERGY_OFFSET       0               ///< Event energy offset


// Settings for the trace size.
#define MAX_TRACELENGTH 2000    ///< Maximum trace length that can be stored in the PixieEvent format.

#endif // PIXIESETTINGS_H
