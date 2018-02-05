#include "explore.h"

int main(int argc, char *argv[])
{
	if (argc < 1) {
		std::cout << "usage: <executable> <file name>";
		return 0;
	}
	explore::eeg_forward_type f;
	explore::eeg_parser_type p(f);

	explore::eeg_file_client fc(argv[0], p);

	f.on_error([&](const std::string &msg)->void {
		std::cout << msg << std::endl;
	});
	f.on_sens([](explore::sens_packet &&sens)->void {
		std::cout << sens.timestamp << '\t'
			<< sens.data[0].temperature << std::endl;
	});
	f.on_acc([](explore::acc_packet &&acc)->void {
		std::cout << acc.data[0].ax << '\t'
			<< acc.data[0].ay << '\t'
			<< acc.data[0].az << std::endl;
	});
	f.on_eeg8([](explore::eeg8_packet &&d)->void {
		for (auto v : d.data.back().data)
			std::cout << v << '\t';
		std::cout << std::endl;
	});

	fc.start();
	return 0;
}
