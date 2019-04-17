# libexplorecpp
Simple Sample C++ library to talk to Mentalab biosignal aquisition devices 


## Prequesites:

GCC 4.9.1 or higher (using --std=c++1y )

Microsoft Visual Studio 2015 or highter

Boost C++ Libraries http://www.boost.org

## Windows Installation (boost dependency)

clone vcpkg repository from https://github.com/Microsoft/vcpkg.git

In command prompt, run the following commands:

`cd vcpkg`

`bootstrap-vcpkg.bat`

`vcpkg integrate install`

`vcpkg install boost-asio:x64-windows`  

`vcpkg install boost-endian:x64-windows`  


## Issues



## Quickstart

add "include/" to include directories

link with libboost_asio

or use included CMake file for some samples

pair the device

use outbound serial port as connection

``` c++
#include "include/explore.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "include/data_parser.h"


int main()
{
	explore::eeg_forward_type f;
	explore::eeg_parser_type p(f);
	explore::eeg_serial eeg("COM4", p);

	f.on_error([&](const std::string &msg)->void {
		eeg.stop();
		std::cout << msg << std::endl;
	});
	f.on_sens([](explore::sens_packet &&sens)->void {
		std::cout << "Timestamp: " 
			<< sens.timestamp << '\t'
			<< "Temperature: "
			<< sens.data[0].temperature << std::endl;
	});
	f.on_acc([](explore::acc_packet &&acc)->void {
		std::cout << "Accelerometer: " 
			<< acc.data[0].ax << '\t'
			<< acc.data[0].ay << '\t'
			<< acc.data[0].az << std::endl;
	});
	f.on_eeg8([](explore::eeg8_packet &&d)->void {
		explore::eeg8_parser ps8;
		ps8.parse(d);

		for (const auto &r : ps8.values) {
			std::cout << "ExG8 packet: " << std::endl;
			for (const auto &v : r)
				std::cout << v << " ";
			std::cout << std::endl;
		}
	});
	f.on_eeg4([](explore::eeg4_packet &&d)->void {
		explore::eeg4_parser ps4;
		ps4.parse(d);

		for (const auto &r : ps4.values) {
			std::cout << "ExG4 packet: " << std::endl;
			for (const auto &v : r)
				std::cout << v << " ";
			std::cout << std::endl;
		}
	});
	f.on_info([](explore::dev_info_packet &&info)->void {
		std::cout << "Firmware version: "
			<< info.version << std::endl;
	});

	eeg.start();

	std::string console;
	std::cin >> console;
	return 0;
}
```

