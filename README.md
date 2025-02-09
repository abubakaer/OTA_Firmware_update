# OTA_Firmware_update
This project enables over-the-air (OTA) firmware updates for an ESP32-based device via a Wi-Fi network. The device acts as a TCP server, listens for specific commands, and performs a firmware update when the "UPDATE" command is received from a client. The firmware file is fetched from a Google Drive direct download link.
