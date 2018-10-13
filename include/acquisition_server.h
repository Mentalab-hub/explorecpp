#include <iostream>
#include "explore.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include "data_parser.h"

#include "lsl_cpp.h"
#include <stdlib.h>




class acquisition_server {
	//char *ch_list[] = { "ch01","ch02","ch03","ch04","ch05","ch06","ch07","ch08" };
	std::vector<const char*> eeg_chan_list = { "ch01","ch02","ch03","ch04","ch05","ch06","ch07","ch08" };
	std::vector<const char*> orn_chan_list = { "ax","ay","az","ga","gy","gz","ma","my","mz" };
	using serial_dev = explore::eeg_serial_base<explore::base_parser<acquisition_server>>;
	serial_dev::parser_type p;
	serial_dev eeg;
	std::string eeg_stream_name = "Explore1", eeg_stream_type = "EEG";
	std::string orn_stream_name = "Explore2", orn_stream_type = "ORN";
	int eeg_srate = 250, orn_srate=23; //Sampling rate in Hz
	int eeg_nchan=4, orn_nchan=9;		// Number of channels
	lsl::stream_info eeg_info, orn_info;
	lsl::stream_outlet eeg_outlet, orn_outlet;

public:
	acquisition_server(const std::string &dev) : 
		p(*this),
		eeg(dev, p), 
		eeg_info(eeg_stream_name, eeg_stream_type, eeg_nchan, eeg_srate, lsl::cf_float32, std::string(eeg_stream_name) += eeg_stream_type),
		eeg_outlet(eeg_info), 
		orn_info(orn_stream_name, orn_stream_type, orn_nchan, orn_srate, lsl::cf_float32, std::string(orn_stream_name) += orn_stream_type),
		orn_outlet(orn_info) {}

	void start() { eeg.start(); }

	void stop() { eeg.stop(); }

	void on_error(std::string &&msg) {
		eeg.stop();
		std::cout << msg << std::endl;
	}

	void on_sens(explore::sens_packet &&sens) {
		std::cout << "sens:" << sens.timestamp << '\t'
			<< sens.data[0].temperature << std::endl;
	}
	void on_eeg4(explore::eeg4_packet &&d) {
		explore::eeg4_parser ps4;
		ps4.parse(d);
		for (const auto &r : ps4.values) {
			std::cout << "EEG4 packet: ";
			float sample[4];
			int i = 0;
			for (const auto &v : r) {
				std::cout << v << " ";
				sample[i++] = v;
			}
			eeg_outlet.push_sample(sample);
			std::cout << std::endl;
		}
	}
	void on_eeg8(explore::eeg8_packet &&d) {
		std::cout << "EEG8 packet: ";
		for (auto v : d.data.back().data)
			std::cout << v << '\t';
		std::cout << std::endl;
	}
	void on_acc(explore::acc_packet &&acc) {
		std::cout << "ACC:" << acc.data[0].ax << '\t'
			<< acc.data[0].ay << '\t'
			<< acc.data[0].az << '\t'
			<< acc.data[0].wx << '\t'
			<< acc.data[0].wy << '\t'
			<< acc.data[0].wz << '\t'
			<< acc.data[0].mx << '\t'
			<< acc.data[0].my << '\t'
			<< acc.data[0].mz << std::endl;
		int sample[9];
		sample[0] = acc.data[0].ax;
		sample[1] = acc.data[0].ay;
		sample[2] = acc.data[0].az;
		sample[3] = acc.data[0].wx;
		sample[4] = acc.data[0].wy;
		sample[5] = acc.data[0].wz;
		sample[6] = acc.data[0].mx;
		sample[7] = acc.data[0].my;
		sample[8] = acc.data[0].mz;
		orn_outlet.push_sample(sample);
	}
	void on_connect(const bool b) {
		eeg_info.desc().append_child_value("manufacturer", "Mentalab");
		eeg_info.desc().append_child_value("device", "Explore");
		lsl::xml_element eeg_chns = eeg_info.desc().append_child("channels");
		for (int k = 0; k < eeg_nchan; k++)
			eeg_chns.append_child("channel")
			.append_child_value("label", eeg_chan_list[k])
			.append_child_value("unit", "volts")
			.append_child_value("type", "EEG");

		orn_info.desc().append_child_value("manufacturer", "Mentalab");
		orn_info.desc().append_child_value("device", "Explore");
		lsl::xml_element orn_chns = orn_info.desc().append_child("channels");
		for (int k = 0; k < eeg_nchan; k++)
			orn_chns.append_child("channel")
			.append_child_value("label", orn_chan_list[k])
			.append_child_value("unit", "volts")
			.append_child_value("type", "EEG");
	}
};


