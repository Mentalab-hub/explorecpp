#pragma once

/*
	C++ libexplore data types
*/

#include <stdint.h>
#include <vector>

namespace explore {

#pragma pack(push,1)
	template<size_t _Size>
	struct eeg_sample {
		int32_t status;
		std::array<int32_t,_Size> data;
		constexpr size_t size()const { return data.size(); }
	};

	struct acc_sample {
		int32_t ax;
		int32_t ay;
		int32_t az;
		int32_t wx;
		int32_t wy;
		int32_t wz;
		int32_t mx;
		int32_t my;
		int32_t mz;
		constexpr size_t size()const { return 9; }
	};

	struct sens_sample {
		int32_t temperature;
		uint32_t light;
		uint32_t battery;
		constexpr size_t size()const { return 5; }
	};

	typedef std::vector<eeg_sample<4>> eeg4_array;
	typedef std::vector<eeg_sample<8>> eeg8_array;
	typedef std::vector<acc_sample> acc_array;
	typedef std::vector<sens_sample> sens_array;


	struct sens_packet {
		uint32_t timestamp;
		sens_array data;
	};

	struct acc_packet {
		uint32_t timestamp;
		acc_array data;
	};

	struct eeg4_packet {
		uint32_t timestamp;
		eeg4_array data;
	};

	struct eeg8_packet {
		uint32_t timestamp;
		eeg8_array data;
	};


#pragma pack(pop)
}
