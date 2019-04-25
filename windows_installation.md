# Libexplorecpp installation on Windows

This tutorial guides you to install libexplorecpp and create a project with it.


## Installation

* Install Microsoft Visual Studio 2015 or higher
* Install boost library
	* Download vcpkg repository manually from [here](https://github.com/Microsoft/vcpkg) or using git with this command git clone https://github.com/Microsoft/vcpkg.git
	* Open a command prompt window with administrator permission.
	* Go to vcpkg folder with `cd vcpkg`
	* run the following commands for installation of Boost library.
		* `bootstrap-vcpkg.bat`
		* `vcpkg integrate install`
		* `vcpkg install boost-asio:x64-windows` OR `vcpkg install boost-asio:x86-windows`
		* `vcpkg install boost-endian:x64-windows` OR `vcpkg install boost-endian:x86-windows`

Now, you have installed all the requirements.

## Create project

* In Visual Studio, create a new "Windows console application".
* Copy [include](https://github.com/Mentalab-hub/libexplorecpp/tree/master/include) folder of libexplorecpp in the project folder.
* In project properties → Configuration manager, change the platform to x86 or x64 depending on your boost installation architecture.
* Add the inlcude folder to your project. (Project properties → C/C++ → Additional include libraries → add $(ProjectDir)include.
* Make sure using precompiled header is off. (Project properties → C/C++ → Precompiled headers → Precompiled header → Not using Precompiled headers).

Pair Explore device with your laptop and find the COM port of the device in the Windows device manager in Ports (COM & LPT). If it is necassary, change the "COM4" port in the code bellow. Now you should be able to build and run the following code. 

```c++
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

 
