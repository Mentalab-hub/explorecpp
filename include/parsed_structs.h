#pragma once

/*
	C++ libexplore data types
*/

#include <stdint.h>
#include <vector>

namespace explore {

#pragma pack(push,1)

	template<typename _Array>
	struct packet_base {
		uint32_t timestamp;
		_Array data;

		static constexpr size_t size() { return std::tuple_size<_Array>::value; }
	};

	struct acc_sample {
		union {
			struct {
				int32_t ax;
				int32_t ay;
				int32_t az;
				int32_t wx;
				int32_t wy;
				int32_t wz;
				int32_t mx;
				int32_t my;
				int32_t mz;
			};
			std::array<int32_t, 9> values;
		};
	};

	using acc_array = std::array<acc_sample, 1>;
	using acc_packet = packet_base<acc_array>;

	struct sens_sample {
		int32_t temperature;
		uint32_t light;
		uint32_t battery;
	};

	using sens_array = std::array<sens_sample, 1>;
	using sens_packet = packet_base<sens_array>;

	template<size_t _Channels>
	struct eeg_sample {
		int32_t status;
		std::array<int32_t, _Channels> data;

		static constexpr size_t channels() { return _Channels; }
	};

#define EEG_PACKET_SIZE 128

	template<size_t _Channels, 
		size_t _Size = EEG_PACKET_SIZE / _Channels,
		typename _Sample = eeg_sample<_Channels>>
	struct eeg_base 
		: public packet_base<std::array<_Sample, _Size>> {
		typedef _Sample sample_type;

		float gain = 6.0f;
		float vref = 4.5f;

		static constexpr size_t channels() { return _Channels; }
		static constexpr size_t size() { return _Size; }
	};

	using eeg4_packet = eeg_base<4>;
	using eeg8_packet = eeg_base<8> ;

#pragma pack(pop)
}
