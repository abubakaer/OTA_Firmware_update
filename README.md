OTA Firmware Update - ESP32 Project
<br>
Overview
<br>
This project enables over-the-air (OTA) firmware updates for an ESP32-based device via a Wi-Fi network. The device acts as a TCP server, listens for specific commands, and performs a firmware update when the "UPDATE" command is received from a client. The firmware file is fetched from a Google Drive direct download link.

Features
<br>
Wi-Fi Connectivity: 
<br>
Connects to a Wi-Fi network using credentials defined in the code.
<br>
TCP Server: 
<br>
Listens for incoming client connections on port 90.
<br>
Firmware Versioning: 
<br>
Displays the current firmware version in the serial monitor.
<br>
OTA Update: 
<br>
Performs a firmware update if the "UPDATE" command is received from the client.
<br>
EEPROM Flag: 
<br>
Uses EEPROM to store a flag indicating if an update is required, ensuring that the firmware update happens only once.
<br>
Remote Firmware URL: 
<br>
The firmware is fetched from a direct Google Drive download link.
<br>
Requirements
<br>
ESP32 Board: This code is designed for the ESP32 platform.
<br>
Arduino IDE: Install the necessary libraries in the Arduino IDE:
<br>
WiFi.h for Wi-Fi connectivity.
<br>
EEPROM.h for reading and writing flags to EEPROM.
<br>
HTTPClient.h for making HTTP requests.
Update.h for handling OTA firmware updates.
