#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 Configuration
#define ONE_WIRE_PIN 4
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

// LCD Configuration
#define LCD_ADDRESS 0x27  // Use I2C Scanner to find correct address
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void setup() {
    Serial.begin(115200);
    Serial.println("DS18B20 Temperature Display");

    // Initialize I2C
    Wire.begin(21, 22);

    // TODO: Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Temp Monitor");

    // TODO: Initialize DS18B20
    sensors.begin();
    // Print number of devices found:
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount());
    Serial.println(" devices");
}

void loop() {
    // TODO: Request temperature reading
     sensors.requestTemperatures();

    // TODO: Read temperature in Celsius
     float tempC = sensors.getTempCByIndex(0);

    // TODO: Display on LCD
     lcd.setCursor(0, 1);  // Second row
     lcd.print("Temp: ");
     lcd.print(tempC, 1);  // 1 decimal place
     lcd.print(" C  ");    // Extra spaces to clear old digits

    // TODO: Also print to Serial Monitor
     Serial.print("Temperature: ");
     Serial.print(tempC);
     Serial.println(" C");

    delay(2000);  // Update every 2 seconds
}