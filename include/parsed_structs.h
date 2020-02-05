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
		double_t temperature;
		double_t light;
		double_t battery;
	};

	using sens_array = std::array<sens_sample, 1>;
	using sens_packet = packet_base<sens_array>;

	struct devinfo_packet {
		uint16_t version;
		uint16_t datarate;
		uint16_t mask;
	};

	//using devinfo_array = std::array<dev_info, 1>;
	//using devinfo_packet = packet_base<dev_info_array>;

	/*
	struct marker_sample {
		uint32_t counter;
	};

	using marker_array = std::array<marker_sample, 1>;
	using marker_packet = packet_base<marker_array>;
	*/
	struct marker_packet {
		uint32_t counter;
	};

	struct calib_info {
		int32_t offset;
		int32_t slope;
	};
	using calib_array = std::array<calib_info, 1>;
	using calib_info_packet = packet_base<calib_array>;

	template<size_t _Channels>
	struct eeg_sample {
		int32_t status;
		std::array<double_t, _Channels> data;

		static constexpr size_t channels() { return _Channels; }
	};

#define EEG_PACKET_SIZE 132

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
	using eeg8_packet = eeg_base<8>;

#pragma pack(pop)
}
