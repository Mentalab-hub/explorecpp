# libexplorecpp
Simlpe Sample C++ library to talk to Mentalab biosignal aquisition devices 


## Prequesites:

GCC 4.9.1 or higher (using --std=c++1y )

Microsoft Visual Studio 2015 or highter

Boost C++ Libraries http://www.boost.org

## Issues

works on Little Endian systems only (yet)

## Quickstart

add "include/" to include directories

link with libboost_asio

or use included CMake file for some samples

``` c++
#include <iostream>
#include "explore.h"

int main()
{
	explore::eeg_forward_type f;
	explore::eeg_parser_type p(f);
	explore::eeg_serial eeg("COM6", p);

	f.on_error([&](const std::string &msg)->void {
		eeg.stop();
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

	eeg.start();

	std::string console;
	std::cin >> console;
	return 0;
}
```

