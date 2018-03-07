#pragma once

/*
	raw data type definitions, if you want to roll your own library
*/

#include <stdint.h>

#pragma pack(push, 1)


// int24_t definition & converter
const size_t INT24_MIN = -8388607;
const size_t INT24_MAX = 8388607;

struct int24_t {
	uint8_t _v[3];

	operator int32_t() const
	{
		return _v[2] & 0x80 ?
			(0xff << 24) | (_v[2] << 16) | (_v[1] << 8) | _v[0] :
			(_v[2] << 16) | (_v[1] << 8) | _v[0];
	}
	int32_t value()const { return (int32_t)*_v; }
};

// header type
typedef struct {
	uint8_t id;
	uint8_t seq;
	uint16_t size;
} eeg_header;

// 4 channel eeg data type
typedef struct{
	int24_t status;
	int24_t data[4];
} raw_eeg_sample4;

// 8 channel eeg data type
typedef struct {
	int24_t status;
	int24_t data[8];
} raw_eeg_sample8;

// accelerometer data type
typedef struct {
	int16_t data[9];
} raw_acc_sample;

// sensors data type
typedef struct  {
	int8_t temperature;
	uint16_t light;
	uint16_t battery;
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
	uint32_t timestamp;
	data_union _data;
} eeg_packet;

#pragma pack(pop)
