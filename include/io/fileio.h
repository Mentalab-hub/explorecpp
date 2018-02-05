#pragma once

#include "base.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace explore {
	// simple file client to read binary dumps
	template<class _Parser, size_t _Headersize, size_t _Offset, typename _OffsetType, size_t _Maxsize = 65535>
	class file_client : private boost::noncopyable {
		_Parser &p_;

		std::string device_;
		std::atomic<bool> brun{ true };
	public:
		explicit file_client(const std::string &device, _Parser &p) : device_(device), p_(p) { }
		file_client(const std::string &device, _Parser &&p) = delete;

		void start() {
			brun = true;
			try {
				std::ifstream str;
				std::vector<uint8_t> b(_Maxsize);
				const uint8_t *ptr = &b[0];
				str.open(device_, std::ios::binary | std::ios::in);
				while (str.good() && brun) {
					str.read((char*)&b[0], _Headersize);
					const int32_t end = *((_OffsetType*)(ptr + _Offset));
					if (end < 0 || end > _Maxsize)
						throw std::runtime_error{ "maxsize limit exceeded" };
					str.read((char*)(ptr + _Headersize), end);
					buffer_pointer _buf = std::make_unique<byte_buffer>(end + _Headersize);
					std::copy(ptr, ptr + end + _Headersize, (char*)_buf->data());
					p_.parse_packet(std::move(_buf));
				}
			}
			catch (std::runtime_error &ex) {
				p_.forward_error(ex.what());
			}
		}
		void stop() { brun = false; }

		const std::string &device()const noexcept { return device_; }
		void device(const std::string &dev) {
			brun = false;
			device_ = dev;
		}
	};
}
