#include "explore.h"

class forward_class {
public:
	void on_error(std::string &&msg) { 
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
	if (argc < 1) {
		std::cout << "usage: <executable> <comdevice>";
		return 0;
	}
	forward_class fw;

	using my_serial = explore::eeg_serial_base<explore::base_parser<forward_class>>;

	my_serial::parser_type p(fw);
	my_serial eeg(argv[0], p);

	eeg.start();

	std::string console;
	std::cin >> console;
	return 0;
}