#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "common.h"
#include <Arduino.h>


const char* mqtt_topic = "sensor3/data";

void setup_wifi();
void reconnect();

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(IP_MQTT_BROKER, PORT_MQTT_BROKER);
}

void setup_wifi() {
  delay(10);
  WiFi.begin(SSID_WIFI, SSID_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  if (Serial.available()) {
    String payload = Serial.readStringUntil('\n');
    if(payload.length() > 0){
      mqttClient.publish(mqtt_topic, payload.c_str());
    }
  }
}
void reconnect() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP01Client")) {
      mqttClient.subscribe(mqtt_topic);
    } else {
      delay(5000);
    }
  }
}
