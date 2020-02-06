# acquisition server
Simple C++ code to help the user have access to the converted packets in the form of data streams compatible with lab streaming layer protocol. you can modify this streams customizing them based on your needs.
You can find the implementation of two data streams, one for ExG data and one for movement and orientation sensors.
````c++
eeg_chan_list = { "ch01","ch02","ch03","ch04","ch05","ch06","ch07","ch08" };
orn_chan_list = { "ax","ay","az","ga","gy","gz","ma","my","mz" };
eeg_stream_name = "Explore1", eeg_stream_type = "EEG";
orn_stream_name = "Explore2", orn_stream_type = "ORN";
```` 
