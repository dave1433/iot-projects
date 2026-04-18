#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

const int LED_PIN = 25;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

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

// TODO: Implement the MQTT message callback
// This function is called when a message arrives on a subscribaed topic
void onMessageReceived(char* topic, byte* payload, unsigned int length) {
    // 1. Convert payload to a String for easier handling
    //    Note: payload is not null-terminated, use the length parameter
    unsigned int payloadLength = length;
    char payloadStr[payloadLength + 1];
    memcpy(payloadStr, payload, payloadLength);
    payloadStr[payloadLength] = '\0';  // Null-terminate the string
    String message = String(payloadStr);

    // 2. Print the topic and message to Serial
    Serial.print("Message received on topic ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(message);

    // 3. Check the message content and control the LED
    //    - "ON" or "1" -> turn LED on
    //    - "OFF" or "0" -> turn LED off
    //    - "TOGGLE" -> toggle current state

    if(message.equalsIgnoreCase("ON") || message.equals("1")) {            
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED turned ON");
    } else if(message.equalsIgnoreCase("OFF") || message.equals("0")) {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED turned OFF");
    } else if(message.equalsIgnoreCase("TOGGLE")) {
        int currentState = digitalRead(LED_PIN);
        digitalWrite(LED_PIN, !currentState);
        Serial.println("LED toggled");
    } else {
        Serial.println("Unknown command");
    }
}

// TODO: Implement MQTT connection and subscription
bool connectMQTT() {
    // 1. Generate a unique client ID
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    // 2. Connect to broker with mqttClient.connect(clientId, MQTT_TOKEN, "")
    //    Note: For Flespi, username is the token, password is empty
    if (mqttClient.connect(clientId.c_str(), MQTT_TOKEN, "")) {
        Serial.println("Connected to MQTT broker");
    } else {
        Serial.print("Failed to connect to MQTT broker, state: ");
        Serial.println(mqttClient.state());
        return false;
    }
    // 3. If connected, subscribe to command topic
        if (mqttClient.subscribe(MQTT_TOPIC_COMMAND)) {
            Serial.print("Subscribed to topic: ");
            Serial.println(MQTT_TOPIC_COMMAND);
        } else {
            Serial.print("Failed to subscribe to topic: ");
            Serial.println(MQTT_TOPIC_COMMAND);
            return false;
        }

    // 4. Return connection status
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("MQTT Subscribe Example");
    Serial.println("======================");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    connectWiFi();

    // TODO: Configure MQTT client
     mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
     mqttClient.setCallback(onMessageReceived);

    // TODO: Connect and subscribe
    connectMQTT();
}

void loop() {
    if (!mqttClient.connected()) {
        Serial.println("Reconnecting to MQTT...");
        connectMQTT();
    }

    mqttClient.loop();
}