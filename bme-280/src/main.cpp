#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

// BME280 I2C address (0x76 or 0x77)
// Check with I2C scanner if unsure
#define BME280_ADDRESS 0x76

#define WINDOW_SIZE 5

// Sea level pressure for altitude calculation (hPa)
#define SEALEVELPRESSURE_HPA 1013.25

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
Adafruit_BME280 bme;
unsigned long lastSwitch = 0;
bool showMainScreen = true;
bool isHot = false;
// TODO: Implement sensor initialization
bool initSensor() {
    // 1. Initialize I2C with Wire.begin()
    Wire.begin();

    // 2. Initialize BME280 with bme.begin(address)
    //    Returns false if sensor not found
    if (!bme.begin(BME280_ADDRESS)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        return false;
    }


    // 3. Configure sensor settings (optional):
    //    bme.setSampling(mode, tempSampling, pressSampling,
    //                    humSampling, filter, duration);

    return true;
}

float tempBuffer[WINDOW_SIZE] = {0};
int tempIndex = 0;

float calculateDewPoint(float temperature, float humidity) {
    // Approximation formula for dew point
    float a = 17.27;
    float b = 237.7;
    float alpha = ((a * temperature) / (b + temperature)) + log(humidity / 100.0);
    return (b * alpha) / (a - alpha);
}

float calculateHeatIndex(float T, float RH) {
    // Heat index formula (simplified)
    return T + 0.33 * RH - 0.70;
}

float movingAverage(float newValue) {
    tempBuffer[tempIndex] = newValue; // Add new value to buffer
    tempIndex = (tempIndex + 1) % WINDOW_SIZE; // Increment index and wrap around

    float sum = 0;// Calculate sum of values in buffer
    for (int i = 0; i < WINDOW_SIZE; i++) {
        sum += tempBuffer[i];// Sum all values in the buffer
    }
    return sum / WINDOW_SIZE;// Return average
}

void updateLCD(float temp, float hum, float dewPoint, float pressure) {
    unsigned long currentTime = millis();
    lcd.clear();

    if (currentTime - lastSwitch > 3000) {
        showMainScreen = !showMainScreen;
        lastSwitch = currentTime;
    }
    if (temp > 30.0) {
       lcd.setCursor(0, 0);
       lcd.print("!!HOT ALERT!! ");
       lcd.setCursor(0, 1);
       lcd.print("Temp:");
       lcd.print(temp, 1);
       lcd.print("C   ");
    return;
    }
    if (showMainScreen) {
        lcd.setCursor(0, 0);
        lcd.print("T:");
        lcd.print(temp, 1);
        lcd.print("C     ");

        lcd.setCursor(0, 1);
        lcd.print("H:");
        lcd.print(hum, 1);
        lcd.print("%     ");
    } else {
        lcd.setCursor(0, 0);
        lcd.print("Dew:");
        lcd.print(dewPoint, 1);
        lcd.print("C    ");

        lcd.setCursor(0, 1);
        lcd.print("P:");
        lcd.print(pressure, 0);
        lcd.print("hPa   ");
    }
}

void readSensor() {
    float temp = bme.readTemperature();
    float smoothTemp = movingAverage(temp);
    float hum = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;
    float dewPoint = calculateDewPoint(temp, hum);

    Serial.println("----- RAW DATA -----");

    Serial.print("Temp: "); Serial.println(temp);
    Serial.print("Smoothed Temp: "); Serial.println(smoothTemp);
    Serial.print("Hum: "); Serial.println(hum);
    Serial.print("Pressure: "); Serial.println(pressure);
    Serial.print("Dew Point: "); Serial.println(dewPoint);

    Serial.println();

    Serial.printf("%lu,%.2f,%.2f,%.2f\n",
              millis(), smoothTemp, hum, pressure);

    // 🔥 HYSTERESIS (runs EVERY time)
    if (smoothTemp > 30.0) {
        isHot = true;
    } else if (smoothTemp < 28.0) {
        isHot = false;
    }

    // Debug
    if (isHot) {
        Serial.println("⚠️ HOT ALERT ACTIVE");
    }

    // 🔥 ALWAYS update LCD
    updateLCD(smoothTemp, hum, dewPoint, pressure);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    lcd.init();
    lcd.backlight();

    Serial.println("BME280 Sensor Example");
    Serial.println("=====================");

    // TODO: Initialize sensor
    // If initialization fails, print error and halt

    if (!initSensor()) {
        Serial.println("Failed to initialize sensor!");
        while (1) {
            // Halt
        }
    }

    Serial.println("Sensor initialized successfully!");
    Serial.println();
}

void loop() {
    // TODO: Read and display sensor data

    readSensor();

    delay(2000);
}