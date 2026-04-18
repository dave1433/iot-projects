#include <Arduino.h>

// RGB LED Pins
const int RED_PIN = 25;
const int GREEN_PIN = 26;
const int BLUE_PIN = 27;

//Pot pins
const int POT_RED = 34;
const int POT_GREEN = 35;
const int POT_BLUE = 32;

//CHANNELs
const int RED_CH = 0;
const int GREEN_CH = 1;
const int BLUE_CH = 2;

// PWM Configuration
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;  // 0-255

void setColor(int red, int green, int blue) {
    ledcWrite(RED_CH, red);
    ledcWrite(GREEN_CH, green);
    ledcWrite(BLUE_CH, blue);
}

void setup() {
    Serial.begin(115200);
    Serial.println("RGB LED Color Mixing");

    //Configure channels
     ledcSetup(RED_CH, PWM_FREQ, PWM_RESOLUTION);
     ledcSetup(GREEN_CH, PWM_FREQ, PWM_RESOLUTION);
     ledcSetup(BLUE_CH, PWM_FREQ, PWM_RESOLUTION);

    // Attach pins
    ledcAttachPin(RED_PIN, RED_CH);
    ledcAttachPin(GREEN_PIN, GREEN_CH);
    ledcAttachPin(BLUE_PIN, BLUE_CH); 

    setColor(0,0,0);
}

void setHSV(float h, float s, float v) {
  float r, g, b;

  int i = int(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }

  setColor(int(r * 255), int(g * 255), int(b * 255));
}

void showPreset(int preset){
  switch(preset){
  case 0: setColor(255, 180, 100); break;
  case 1: setColor(180, 220, 255); break;
  case 2: setColor(255, 80, 30); break;
  case 3: setColor(120, 0, 255); break;
  case 4: setColor(0, 255, 180); break;
  
  default: setColor(0, 0, 0);break;
  }

}

void loop() {
    int redValue = analogRead(POT_RED);
    int greenValue = analogRead(POT_GREEN);
    int blueValue = analogRead(POT_BLUE);

    int red = map(redValue, 0, 4095, 0, 255);
    int green = map(greenValue, 0, 4095, 0, 255);
    int blue = map(blueValue, 0, 4095, 0, 255);

    setColor(red, green, blue);

    delay(20);
}