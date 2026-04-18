

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

// TODO: Implement the WiFi event handler

//
// Note: Event constant names depend on ESP32 Arduino core version:
//   Arduino-ESP32 2.x: ARDUINO_EVENT_WIFI_STA_CONNECTED, etc.
//   Arduino-ESP32 1.x: SYSTEM_EVENT_STA_CONNECTED, etc.
// PlatformIO with espressif32 platform 5.x+ uses the ARDUINO_EVENT_* names.

const int LED_PIN = 2; // GPIO pin for LED indicator (optional)

//Fallback WiFi credentials if not defined in config.h
const char* AP_SSID = "ESP32-AP";
const char* AP_PASSWORD = "12345678"; // Minimum 8 characters for WPA2

//Connection timeout and blink interval
const unsigned long WIFI_TIMEOUT = 10000; // WiFi connection timeout in milliseconds
const unsigned long BLINK_INTERVAL = 300; // Blink interval in milliseconds

enum ConnectionStatus{
    STATUS_IDLE,
    STATUS_CONNECTING,
    STATUS_CONNECTED,
    STATUS_DISCONNECTED,
    STATUS_AP_MODE
};

ConnectionStatus currentStatus = STATUS_IDLE;
unsigned long connectionStartTime = 0;

bool isWiFiConnected(){
    return currentStatus == STATUS_CONNECTED && WiFi.status() == WL_CONNECTED;
}

void blinkConnectingLED(){
    static unsigned long lastBlinkTime = 0;
    static bool ledState = false;

    if (millis() - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = millis();
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
    }
    
}

void startAPMode(){
    Serial.println("Starting fallback Access Point mode...");
    WiFi.disconnect(true, true); // Disconnect and erase previous WiFi credentials
    delay(1000); // Short delay to ensure disconnection
    
    WiFi.mode(WIFI_AP);

    if(WiFi.softAP(AP_SSID, AP_PASSWORD)){
        currentStatus = STATUS_AP_MODE;
        digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate AP mode

        Serial.println("Access Point started successfully.");
        Serial.print("AP Name: ");
        Serial.println(AP_SSID);
        Serial.println("AP IP Address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start Access Point mode.");
    }
    
    currentStatus = STATUS_AP_MODE;
}

// This function is called when WiFi events occur (connect, disconnect, got IP)
void onWiFiEvent(WiFiEvent_t event) {
    // Handle these events:
    switch(event){
      case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("WiFi connected");
        currentStatus = STATUS_CONNECTING;
        break;
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        currentStatus = STATUS_CONNECTED;
        digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate successful connection
        Serial.println(WiFi.localIP());

        Serial.println("Signal strength (RSSI): ");
        Serial.println(WiFi.RSSI());
        Serial.println("dBm");
        break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("WiFi disconnected");
        currentStatus = STATUS_DISCONNECTED;
        digitalWrite(LED_PIN, LOW); // Turn off LED when disconnected
        break;
      default:
        break;  
    }
}

// TODO: Implement the connect function
// Should initiate connection and wait until connected or timeout
bool connectToWiFi() {

    Serial.println("Connecting to WiFi...");
    currentStatus = STATUS_CONNECTING;
    connectionStartTime = millis();

    // 1. Set WiFi mode to station (WIFI_STA)
     WiFi.mode(WIFI_STA);
    // 2. Begin connection with WiFi.begin(ssid, password)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // 3. Wait for connection with timeout (e.g., 10 seconds)
   

     while(WiFi.status() != WL_CONNECTED && millis() - connectionStartTime < WIFI_TIMEOUT) {
        blinkConnectingLED(); // Blink LED while trying to connect
        delay(300); // Short delay to avoid busy-waiting
    }

     unsigned long connectionTime = millis() - connectionStartTime;

        Serial.print("Connection took: ");
        Serial.print(connectionTime);
        Serial.println(" ms");

    // 4. Return true if connected, false if timeout
    if(WiFi.status() == WL_CONNECTED) {
        currentStatus = STATUS_CONNECTED;

        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        Serial.println("Signal strength (RSSI): ");
        Serial.println(WiFi.RSSI());
        Serial.println("dBm");

        digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate successful connection
        return true;
    }
    currentStatus = STATUS_DISCONNECTED;
    digitalWrite(LED_PIN, LOW); // Ensure LED is off if connection failed
    return false;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 WiFi Connection Example");
    Serial.println("============================");

    // TODO: Register the WiFi event handler
    WiFi.onEvent(onWiFiEvent);

    // TODO: Call connectToWiFi() and handle result
    bool isConnected =connectToWiFi();
    if(isConnected) {
        Serial.println("Successfully connected to WiFi!");
    } else {
        startAPMode(); // Start AP mode if connection failed
    }
  
}

void loop() {
    // TODO: Check connection status periodically
    // This will trigger events if status changes
    if (!isWiFiConnected() && currentStatus != STATUS_AP_MODE){

      Serial.println("WiFi not connected!Trying to reconnect...");
      // If disconnected, attempt to reconnect
      bool reconnected = connectToWiFi();
      if (!reconnected) {
        Serial.println("Reconnect failed. Starting AP mode...");
        startAPMode();
      }
      
    }

    if(isWiFiConnected()){
        Serial.print("Current RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    
    }
    

    delay(5000);
}
