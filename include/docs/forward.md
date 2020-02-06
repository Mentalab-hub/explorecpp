# forward
Simple C++ library to help the user have access to the converted packets in real time: the forwarded events are categorized in several types:
````c++
void on_connect(const bool b) {}
void on_error(const std::string &err) {}
void on_sens(sens_packet &&data) {}
void on_eeg4(eeg4_packet &&data) {}
void on_eeg8(eeg8_packet &&data) {}
void on_acc(acc_packet &&data) {}
void on_info(devinfo_packet &&data) {}
void on_marker(marker_packet&& data) {}
void on_calib_info(calib_info_packet&& data) {}
```` 
user can define the program actions upon receiving this data. for example user can modify the code so that after receiving a marker a visual feedback is given to the subject. The data structure can be found in parsed_structs.h
