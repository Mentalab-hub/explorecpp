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

	typedef std::array<acc_sample, 1> acc_array;
	typedef packet_base<acc_array> acc_packet;

	struct sens_sample {
		int32_t temperature;
		uint32_t light;
		uint32_t battery;
	};

	typedef std::array<sens_sample, 1> sens_array;
	typedef packet_base<sens_array> sens_packet;

	template<size_t _Channels>
	struct eeg_sample {
		int32_t status;
		std::array<int32_t, _Channels> data;

		static constexpr size_t channels() { return _Channels; }
	};

#define EEG_PACKET_SIZE 128

	template<size_t _Channels, size_t _Size = EEG_PACKET_SIZE / _Channels>
	struct eeg_base : public packet_base<std::array<eeg_sample<_Channels>, _Size>> {
		typedef eeg_sample<_Channels> sample_type;

		static constexpr size_t channels() { return sample_type::channels(); }
	};

	typedef eeg_base<4> eeg4_packet;
	typedef eeg_base<8> eeg8_packet;

#pragma pack(pop)
}
