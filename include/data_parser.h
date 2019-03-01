#pragma once

#include "parsed_structs.h"

namespace explore {
	template<typename _Packet,typename _V = double>
	class eeg_data_parser {
	public:
		typedef eeg_base<_Packet::channels()> packet_type;
		typedef _V value_type;
		typedef std::array<std::array<value_type, packet_type::channels()>, packet_type::size()> array_type;

		array_type values;

		static constexpr size_t channels() { return packet_type::channels(); }
		static constexpr size_t size() { return packet_type::size(); }

		void parse(const packet_type &packet) {
			const value_type mult = packet.vref / static_cast<value_type>(0x7fffff) / static_cast<value_type>(packet.gain);
			auto it = packet.data.cbegin();
			for (auto &v : values) {
				auto vit = it->data.cbegin();
				for (auto &p : v) {
					p = mult*static_cast<value_type>(*vit);
					++vit;
				}
				++it;
			}
		}
	};

	using eeg4_parser = typename eeg_data_parser<eeg4_packet>;
	using eeg4_parserf = typename eeg_data_parser<eeg4_packet, float>;

	using eeg8_parser = typename eeg_data_parser<eeg8_packet>;
	using eeg8_parserf = typename eeg_data_parser<eeg8_packet, float>;
}
