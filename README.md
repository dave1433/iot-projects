# ESP32 IoT Projects Collection

A collection of practical IoT projects and experiments developed using **PlatformIO** and the **ESP32** microcontroller (specifically optimized for the DFRobot FireBeetle 2 ESP32-E).

## 🚀 Projects Overview

| Project | Description |
| :--- | :--- |
| **analog-sensor** | Reading analog data (e.g., LDR/Light sensors) with noise filtering. |
| **bme-280** | Temperature, humidity, and pressure monitoring using the BME280 I2C sensor. |
| **i2c-lcd** | Displaying custom characters and sensor data on an I2C 16x2 LCD. |
| **i2c-scanner** | Utility to detect I2C addresses of connected peripherals. |
| **mqtt-publish** | Publishing sensor data to an MQTT broker (e.g., Flespi). |
| **mqtt-subscribe** | Controlling the ESP32 via MQTT messages and commands. |
| **rgb-led-controller** | Precise PWM control for RGB LEDs. |
| **stepper-motor** | Driving stepper motors with controlled acceleration and speed. |
| **temperature-display** | A combined project featuring thermal alerts and real-time LCD updates. |
| **wifi-manager** | Robust WiFi connection handling with Access Point (AP) fallback. |

## 🛠️ Hardware Requirements
- **Microcontroller:** [DFRobot FireBeetle 2 ESP32-E](https://www.dfrobot.com/product-2195.html) (or compatible ESP32).
- **Environment:** VS Code + [PlatformIO IDE](https://platformio.org/).

## ⚙️ Setup & Configuration

To protect sensitive information, WiFi and MQTT credentials are not included in this repository.

1.  Navigate to a project folder (e.g., `mqtt-publish/include/`).
2.  Locate the `config.h.example` file.
3.  Duplicate it and rename the copy to `config.h`.
4.  Fill in your actual WiFi SSID, Password, and MQTT credentials:

```cpp
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```

## 📦 How to Build
1. Open the root folder in VS Code with PlatformIO installed.
2. The IDE will automatically detect the sub-projects.
3. Use the PlatformIO project switcher to select the specific module you want to upload.
4. Click **Build** and **Upload**.
