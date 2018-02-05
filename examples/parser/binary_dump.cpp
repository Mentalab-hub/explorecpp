#include "explore.h"

#include <iostream>
#include <fstream>

class binary_dump final : private boost::noncopyable {
	std::ofstream ofs;
public:
	binary_dump(const std::string &fname) {
		ofs.open(fname, std::ios::binary | std::ios::out | std::ios::app);
	}
	~binary_dump() { ofs.close(); }
	void forward_error(std::string &&ex) {
		ofs.close();
	}
	void parse_packet(explore::buffer_pointer &&buf) {
		if(ofs.is_open())
			ofs.write((char*)buf->data(), buf->size());
	}
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "usage: <executable> <comdevice> <filename>";
		return 0;
	}
	using eeg_binary = explore::eeg_serial_base<binary_dump>;

	binary_dump p(argv[1]);
	eeg_binary eeg(argv[0], p);

	eeg.start();

	std::string console;
	std::cin >> console;

	return 0;
}