#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define BUTTON_PIN 4

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
Adafruit_BME280 bme;

int menuIndex = 0;
unsigned long lastUpdate = 0;

byte degreeSymbol[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};

byte dropSymbol[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B01110,
  B00000
};

void setup() {
    Serial.begin(115200);
    delay(2000); // Allow time for Serial to initialize

    Serial.println("System ready. Initializing...");
    Wire.begin(21, 22);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    bme.begin(0x76);

    lcd.init();
    lcd.backlight();
    lcd.createChar(0, degreeSymbol);
    lcd.createChar(1, dropSymbol);

    lcd.setCursor(0, 0);
    lcd.print("System Ready");
    delay(2000);
}

float humBuffer[5] = {0};
int humIndex = 0;

float smoothHumidity(float newValue) {
    humBuffer[humIndex] = newValue;
    humIndex = (humIndex + 1) % 5;

    float sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += humBuffer[i];
    }
    return sum / 5;
}
// ---------- SCREEN 1 ----------
void displayTempHum() {
    float temp = bme.readTemperature();
    float hum = smoothHumidity(bme.readHumidity());

    static float lastHum = -100;

    if (abs(hum - lastHum) > 0.2) {
        lastHum = hum;
    }

    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temp, 1);
    lcd.write(byte(0));
    lcd.print("C   ");

    lcd.setCursor(0, 1);
    lcd.write(byte(1));
    lcd.print("Hum:");
    lcd.print(lastHum, 1);
    lcd.print("%   ");
}

// ---------- SCREEN 2 ----------
float calculateDewPoint(float t, float h) {
    float a = 17.27;
    float b = 237.7;
    float alpha = ((a * t) / (b + t)) + log(h / 100.0);
    return (b * alpha) / (a - alpha);
}

void displayDewPressure() {
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    float dew = calculateDewPoint(temp, hum);

    lcd.setCursor(0, 0);
    lcd.print("Dew:");
    lcd.print(dew, 1);
    lcd.write(byte(0));
    lcd.print("C   ");

    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(pressure, 0);
    lcd.print("hPa   ");
}

// ---------- SCREEN 3 ----------
void displayClock() {
    unsigned long sec = millis() / 1000;
    int min = sec / 60;
    int s = sec % 60;

    lcd.setCursor(0, 0);
    lcd.print("Time:");
    lcd.print(min);
    lcd.print(":");
    if (s < 10) lcd.print("0");
    lcd.print(s);

    lcd.setCursor(0, 1);
    lcd.print("Running...   ");
}

// ---------- SCREEN 4 ----------
void scrollText(String text) {
    static int index = 0;
    static unsigned long lastUpdate = 0;

    if (millis() - lastUpdate > 300) {
        lcd.setCursor(0, 0);
        lcd.print(text.substring(index, index + 16));
        lcd.print("   ");

        index++;
        if (index > text.length()) index = 0;

        lastUpdate = millis();
    }
}

// ---------- LOOP ----------
void loop() {

    // Button
    static bool lastButtonState = HIGH;
    static bool buttonState = HIGH;
    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50;

    bool reading = digitalRead(BUTTON_PIN);

    // If button changed, reset debounce timer
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    // If stable for long enough
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            // Detect press (HIGH → LOW)
           if (buttonState == LOW) {
               menuIndex++;
               Serial.println(menuIndex); // DEBUG
            }
        }
    }

    lastButtonState = reading;

    int currentScreen = menuIndex % 4;

    // Clear only when screen changes
    static int lastMenuIndex = -1;
    if (currentScreen != lastMenuIndex) {
        lcd.clear();
        lastMenuIndex = currentScreen;
    }

    if (millis() - lastUpdate > 500) {

       switch (currentScreen) {
         case 0:
            displayTempHum();
            break;
         case 1:
            displayDewPressure();
            break;
         case 2:
            displayClock();
            break;
         case 3:
            scrollText("Scrolling message demo...");
            break;
        }
       lastUpdate = millis();
    }
}