#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <Update.h>

// Firmware version
#define FIRMWARE_VERSION "0.0.0" // Update this for new firmware versions

// Wi-Fi credentials
const char* ssid = "your ssid";
const char* password = "your password";

WiFiServer tcpServer(90); // Create TCP server on port 90
int updateFlag = 0;
String currentVersion = FIRMWARE_VERSION; // Store the current firmware version

// Google Drive direct download link for the firmware file
const char* firmwareURL = "your drive link ";

// Function declaration
void performOTAUpdate();

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  // Retrieve flag from EEPROM
  EEPROM.get(0, updateFlag);
  Serial.print("Flag on Startup: ");
  Serial.println(updateFlag);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  tcpServer.begin(); // Start the TCP server

  // Check if update is flagged
  if (updateFlag == 1) {
    Serial.println("Update flag detected. Starting firmware update...");
    performOTAUpdate();
    updateFlag = 0; // Reset the flag
    EEPROM.put(0, updateFlag);
    EEPROM.commit();
    Serial.println("Update complete. Restarting...");
    ESP.restart();
  }
}

void loop() {
  // Print current firmware version every 5 seconds
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 5000) { // Delay of 5 seconds
    previousMillis = currentMillis;
    Serial.println("Current Firmware Version: " + currentVersion);
  }

  // Check for client connection
  WiFiClient client = tcpServer.available();
  if (client) {
    Serial.println("Client Connected!");

    unsigned long clientStartTime = millis();
    while (client.connected() && (millis() - clientStartTime) < 30000) { // 30-second timeout
      if (client.available()) {
        String command = client.readStringUntil('\n');
        command.trim(); // Remove trailing whitespace or newline characters
        Serial.println("Command Received: " + command);

        // Convert command to uppercase for uniformity
        command.toUpperCase();

        if (command == "UPDATE") {
          updateFlag = 1;
          if (updateFlag != EEPROM.read(0)) { // Avoid unnecessary writes
            EEPROM.put(0, updateFlag);
            EEPROM.commit();
          }
          Serial.println("Flag set. Restarting for update...");
          client.println("Flag set for update.");
          ESP.restart();
        } else {
          client.println("Unknown command.");
          Serial.println("Unknown command received: " + command);
        }
      }
    }

    client.stop();
    Serial.println("Client Disconnected.");
  }
}

// Function to perform OTA update
void performOTAUpdate() {
  HTTPClient http;
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS); // Ensure redirects are followed
  http.setTimeout(60000); // 60 seconds timeout
  http.begin(firmwareURL); // Initialize HTTP connection

  int httpCode = http.GET(); // Make GET request

  if (httpCode == HTTP_CODE_OK) { // HTTP OK (200)
    int contentLength = http.getSize();

    // Check if enough memory and space are available
    if (ESP.getFreeHeap() < 2048) {
      Serial.println("Insufficient memory for update.");
      return;
    }

    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      Serial.println("Beginning OTA update...");
      WiFiClient& client = http.getStream();
      size_t written = 0;
      unsigned long startTime = millis();

      while (written < contentLength && (millis() - startTime) < 60000) { // 60-second timeout
        if (WiFi.status() != WL_CONNECTED) {
          Serial.println("Wi-Fi connection lost during OTA update.");
          Update.abort();
          http.end();
          updateFlag = 0;
          EEPROM.put(0, updateFlag);
          EEPROM.commit();
          Serial.println("Update aborted. Restarting...");
          ESP.restart();
        }
        written += Update.writeStream(client);
      }

      if (written == contentLength) {
        Serial.println("Firmware written successfully.");
      } else {
        Serial.printf("Written only %d/%d bytes. Update failed!\n", written, contentLength);
      }

      if (Update.end()) {
        if (Update.isFinished()) {
          Serial.println("Update successfully completed.");
        } else {
          Serial.println("Update not finished. Something went wrong!");
        }
      } else {
        Serial.printf("Error Occurred: %s\n", Update.errorString());
      }
    } else {
      Serial.println("Not enough space to begin OTA update.");
    }
  } else {
    Serial.printf("HTTP error: %d\n", httpCode); // Log HTTP error code
  }

  http.end(); // Close the connection
}
