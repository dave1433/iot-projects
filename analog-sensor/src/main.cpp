#include <Arduino.h>

const int LDR_PIN = 34;

void setup() {
    Serial.begin(115200);
    pinMode(LDR_PIN, INPUT);
}

void loop() {
    int rawValue = analogRead(LDR_PIN);

    // Convert to percentage (0–100)
    int percentage = map(rawValue, 0, 4095, 0, 100);

    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print(" | Light level: ");
    Serial.print(percentage);
    Serial.println("%");

    delay(500);
}