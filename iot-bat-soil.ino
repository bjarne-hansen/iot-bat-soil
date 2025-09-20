/*

This project has been developed using Visual Studio Code and Arduino CLI.
The code for this project is in main.h and main.cpp.

This application is designed to read soil moisture data from a capacitive soil
moisture sensor and publish readings via BLE so that they can be consumed by the
Bluetooth Gateway and sent to a MQTT broker.

The code is built using the SEEED Studion XIAO ESP32 C3 board, but it should 
works with most other ESP32 boards as well. However, experience shows that there
are often differences in how deep sleep and wake-up works between different 
ESP32 boards.

Because of the low power design and the inherent deep-sleep cycle this solution
does not utilise a configuration file, but uses defined values at the top of 
main.cpp.

See main.cpp for more information.

*/
