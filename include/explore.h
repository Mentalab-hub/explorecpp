#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif // #ifdef _WIN32


#include "io/serial.h"
#include "io/fileio.h"
#include "forward.h"
#include "parser.h"

namespace explore {
	// generic serial client defition; <parser,head size,header offset,offset type>
	template<class _Parser>
	using eeg_serial_base = serial_client<_Parser, sizeof(eeg_header), 2, uint16_t>;

	// explore serial client type
	using eeg_serial = eeg_serial_base<base_parser<base_forward<base_function>>>;
	// default parser type
	using eeg_parser_type = eeg_serial::parser_type;
	// default forward type 
	using eeg_forward_type = eeg_serial::parser_type::forward_type;

	// explore serial thread safe client type
	using eeg_serial_mt = eeg_serial_base<base_parser<base_forward<base_threadsafe_function>>>;

	// simple base parser for 1:1 file dump
	template<class _Parser>
	using eeg_file_base = file_client<_Parser, sizeof(eeg_header), 2, uint16_t>;
	using eeg_file_client = eeg_file_base<base_parser<base_forward<base_function>>>;
}
