#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "common.h"

// MQTT
#define TOPIC_DISTANCE_SENSOR_1 "sensor1/distance"
#define TOPIC_DISTANCE_SENSOR_2 "sensor2/distance"

// Pins
#define TRIG_PIN 3
#define ECHO_PIN 2


WiFiClient espClientSensor1;
PubSubClient mqttClient(espClientSensor1);

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void reconnectMQTT(String client) {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(client)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void loop_common(String topic, String client) {
  if (!mqttClient.connected(client)) {
    reconnectMQTT();
  }
  mqttClient.loop();

  float distance = measureDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (mqttClient.connected()) {
    mqttClient.publish(topic, String(distance).c_str());
  }

  delay(5000);  
}

void setup_common() {
  Serial.begin(19200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectToWiFi();
  mqttClient.setServer(IP_MQTT_BROKER, PORT_MQTT_BROKER);
}

