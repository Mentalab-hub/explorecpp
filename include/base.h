#pragma once

#include <vector>
#include <memory>

namespace explore {
	// base buffer type defintions
	typedef std::vector<uint8_t> byte_buffer;
	typedef std::unique_ptr<byte_buffer> buffer_pointer;
}