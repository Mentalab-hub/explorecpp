# parser
Simple C++ modules that are responsible for data conversion. the raw binarry data will be converted into human readable data structures. The outputs data structures are available in parsed_structs.h conversion of ExG packets takes place partly in data_parser.h
Here is a list of different packets that will be generated here:
````c++
eeg4_packet: 4 channel devices ExG packet
eeg8_packet: 8 channel devices ExG packet
sens_packet: temperature, battery and light sensor data
acc_packet: orientation and movement sensors packet 
devinfo_packet: firmware version, active channels and data rate information
marker_packet: marker event
```` 