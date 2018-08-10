#include "explore.h"

#include <iostream>

class example {
	using serial_dev = explore::eeg_serial_base<explore::base_parser<example>>;

	serial_dev::parser_type p;
	serial_dev eeg;
public:
	example(const std::string &dev) : p(*this), eeg(dev, p) { }

	void start() { eeg.start(); }

	void stop() { eeg.stop(); }

	void on_error(std::string &&msg) {
		eeg.stop();
		std::cout << msg << std::endl;
	}

	void on_sens(explore::sens_packet &&sens) {
		std::cout << sens.timestamp << '\t'
			<< sens.data[0].temperature << std::endl;
	}
	void on_eeg4(explore::eeg4_packet &&d) {
		for (auto v : d.data.back().data)
			std::cout << v << '\t';
		std::cout << std::endl;
	}
	void on_eeg8(explore::eeg8_packet &&d) {
		for (auto v : d.data.back().data)
			std::cout << v << '\t';
		std::cout << std::endl;
	}
	void on_acc(explore::acc_packet &&acc) {
		std::cout << acc.data[0].ax << '\t'
			<< acc.data[0].ay << '\t'
			<< acc.data[0].az << std::endl;
	}
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "usage: <executable> <comdevice>";
		return 0;
	}	
	example e(argv[1]);

	e.start();

	std::string console;
	std::cin >> console;
}