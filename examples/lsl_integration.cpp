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

	acquisition_server e("COM3");

	e.start();
	std::cout << "Now sending data... " << std::endl;

	std::string console;
	std::cin >> console;

	return 0;
}