# libexplorecpp and LSL integration

This guide helps you to set up a labstreaminglayer (LSL) connection in order to send data acquired by C++ API to other environments such as Matlab, Openvibe, C#, Python, etc..


## Installation

* Follow this [tutorial](https://github.com/Mentalab-hub/libexplorecpp/blob/master/windows_installation.md) to install the C++ API. 
* You need to install LSL on your computer. Follow this [instruction](https://github.com/sccn/labstreaminglayer/blob/master/doc/BUILD.md) to install and build LSL and its dependencies. You might be able to use pre-compiled LSL from [here](ftp://sccn.ucsd.edu/pub/software/LSL/SDK/) instead of installing LSL from scratch. However, these binary releases might not work on all operating systems with different architectures.
* Create a new Visual Studio project and include API library (more details in this [link](https://github.com/Mentalab-hub/libexplorecpp/blob/master/windows_installation.md)). Add “lsl_cpp.h” to the project libraries.
* Copy LSL bin folder (containing LSL binary files) to your project folder and add it to your project libraries (Project properties > Linker > General > Additional Library Directories).
* Use acquisition_server class for acquiring data and sending to LSL as follows.

```C++
#include <iostream>
#include "include/explore.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "include/data_parser.h"
 
#include "lsl_cpp.h"
#include <stdlib.h>
#include <acquisition_server.h>
 
int main(int argc, char* argv[])
{
 
    acquisition_server e("COM4");
 
    e.start();
    std::cout << "Now sending data... " << std::endl;
 
    std::string console;
    std::cin >> console;
 
    return 0;
}
```

## Visualization in Openvibe

* Download and install Openvibe from this [link](http://openvibe.inria.fr/downloads/).
* Start Openvibe acquisition server. Choose lsl driver from the list and in driver properties search for the “Explore_EEG stream”, connect to the stream and play data acquiring.
* In Openvibe designer you can design your scenario to preprocess and visualize the data.

## MATLAB

* If you have installed and built your LSL software with Visual Studio and you are using a 64bit Matlab, you do not have the 64bit LSL libraries in your built project. You need to download the pre-compiled version of lsl-Matlab SDK from [here](ftp://sccn.ucsd.edu/pub/software/LSL/SDK/). If you are using a 32bit version of Matlab, the 32bit LSL libraries will be enough.
* Add liblsl-matlab folder and all subfolders to Matlab path.
* Run one of the MATLAB [examples](https://github.com/Mentalab-hub/explorematlab/tree/master/lsl-acquisition) for getting the data in real time from the device.
* The default setting in the example is for 4-channel device. If you are using an 8-channel device, change the following variables from 4 to 8:
  * Line 31: `block.OutputPort(1).Dimensions = 4; % Number of EEG channels`
  * Line 73:  `block.Dwork(1).Dimensions = 4; % Number of EEG channels`
