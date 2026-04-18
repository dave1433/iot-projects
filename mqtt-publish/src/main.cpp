#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <config.h>


const int LDR_PIN = 34;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Connect to WiFi (you can reuse code from Exercise WiFi Connect)
void connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// TODO: Implement MQTT connection function
// Should connect to broker and return true/false
bool connectMQTT() {
    // 1. Check if already connected with mqttClient.connected()
    if (mqttClient.connected()) {
        return true;
    }
    // 2. Generate a unique client ID (e.g., "esp32-" + random number)
    String clientId = "esp32-" + String(random(0xffff), HEX);
    // 3. Attempt connection with mqttClient.connect(clientId, MQTT_TOKEN, "")
    //    Note: For Flespi, username is the token, password is empty
    if (mqttClient.connect(clientId.c_str(), MQTT_TOKEN, "")) {
        Serial.println("Connected to MQTT broker!");
        return true;
    } else {
        Serial.print("Failed to connect to MQTT broker, state: ");
        Serial.println(mqttClient.state());
    }
    // 4. Print success or failure message
    // 5. Return connection status
    return false;
}

// TODO: Implement function to read sensor and publish
void publishSensorData() {
    // 1. Read the LDR value using analogRead()
    int rawValue = analogRead(LDR_PIN);
    // Convert to percentage (0-100%)
    int percentage = map(rawValue, 0, 4095, 0, 100);
    // 2. Convert to string using String() or snprintf()
    String payload = String(percentage);
    // 3. Publish using mqttClient.publish(topic, payload)
    mqttClient.publish(MQTT_TOPIC, payload.c_str());
    // 4. Print confirmation to Serial
    Serial.println("Published: " + payload + "%");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("MQTT Publish Example");
    Serial.println("====================");

    pinMode(LDR_PIN, INPUT);

    connectWiFi();

    // Configure MQTT client
    mqttClient.setServer("mqtt.flespi.io", 1883);

    // Attempt MQTT connection
    connectMQTT();
}

void loop() {
    
    if(!mqttClient.connected()) {
        connectMQTT();
    }
    mqttClient.loop();

    static unsigned long lastPublishTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastPublishTime >= 5000) { // 5 seconds
        publishSensorData();
    }
    delay(100);
}
