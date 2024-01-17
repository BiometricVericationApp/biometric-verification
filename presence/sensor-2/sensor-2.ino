#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "common.h"
#include "common_ultrasound.h"


WiFiClient espClientSensor2;
PubSubClient mqttClient(espClientSensor2);

void setup() {
  Serial.begin(19200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectToWiFi();
  mqttClient.setServer(IP_MQTT_BROKER, PORT_MQTT_BROKER);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  float distance = measureDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (mqttClient.connected()) {
    mqttClient.publish(TOPIC_DISTANCE_SENSOR_2, String(distance).c_str());
  }

  delay(5000);  
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(SSID_WIFI, SSID_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float time = pulseIn(ECHO_PIN, HIGH);
  float distance = time * 0.0343 / 2;
  return distance;
}
