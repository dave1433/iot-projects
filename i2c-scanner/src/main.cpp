#include <Arduino.h>
#include <Wire.h>

// Default I2C pins for ESP32
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// TODO: Implement function to identify common I2C devices
const char* identifyDevice(int address) {
    // Return device name based on address
    // Common addresses:
    switch(address) {
        case 0x27:
        case 0x3F:
            return "LCD with I2C backpack";
        case 0x3C:
        case 0x3D:
            return "OLED display (SSD1306)";
        case 0x40:
            return "HDC1080 temperature/humidity";
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
            return "ADS1115 ADC";
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
            return "EEPROM";
        case 0x68:
            return "DS3231 RTC or MPU6050";
        case 0x76:
        case 0x77:
            return "BME280 or BMP280";
    }
    return "Unknown";
}

// TODO: Implement function to scan I2C bus
void scanI2C() {
    Serial.println("Scanning I2C bus...\n");

    int devicesFound = 0;

    for (int address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();

        if (error == 0) {
            Serial.printf("I2C device found at 0x%02X - %s\n",
                          address, identifyDevice(address));
            devicesFound++;
        } else if (error == 4) {
            Serial.printf("Unknown error at address 0x%02X\n", address);
        }
    }

    Serial.println();

    if (devicesFound == 0) {
        Serial.println("No I2C devices found!");
    } else {
        Serial.printf("Found %d device(s)\n", devicesFound);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("I2C Scanner");
    Serial.println("============");

    // TODO: Initialize I2C with Wire.begin(SDA, SCL)
    Wire.begin(SDA_PIN, SCL_PIN);

    // TODO: Call scan function
    scanI2C();
}

void loop() {
    static unsigned long lastScanTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastScanTime >= 5000) {
        scanI2C();
        lastScanTime = currentTime;
    }

    delay(100);
}