#pragma once

/*
	raw data type definitions, if you want to roll your own library
*/

#include <stdint.h>
#include <boost/endian/arithmetic.hpp>

#pragma pack(push, 1)


// header type
typedef struct {
	uint8_t id;
	uint8_t seq;
	boost::endian::little_uint16_t size;
} eeg_header;

// 4 channel eeg data type
typedef struct{
	boost::endian::little_int24_t status;
	boost::endian::little_int24_t data[4];
} raw_eeg_sample4;

// 8 channel eeg data type
typedef struct {
	boost::endian::little_int24_t status;
	boost::endian::little_int24_t data[8];
} raw_eeg_sample8;

// accelerometer data type
typedef struct {
	boost::endian::little_int16_t data[9];
} raw_acc_sample;

// sensors data type
typedef struct  {
	int8_t temperature;
	boost::endian::little_uint16_t light;
	boost::endian::little_uint16_t battery;
} raw_sens_sample;

// packet union
typedef union
{
	raw_eeg_sample4 eeg_samples4[32];
	raw_eeg_sample8 eeg_samples8[16];
	raw_acc_sample acc_samples[1];
	raw_sens_sample sens_samples[1];
} data_union;

// packet definition
typedef struct 
{
	eeg_header head;
	boost::endian::little_uint32_t timestamp;
	data_union _data;
} eeg_packet;

#pragma pack(pop)
