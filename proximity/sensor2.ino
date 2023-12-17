#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "privatered";
const char* password = "vfpk0135";

const int trigPin = 3;
const int echoPin = 2;

const char* mqtt_broker = "192.168.11.213";
const int mqtt_port = 1883;
const char* mqtt_topic = "sensor1/distance";

WiFiClient espClientSensor2;
PubSubClient mqttClient(espClientSensor2);

void setup() {
  Serial.begin(19200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectToWiFi();
  mqttClient.setServer(mqtt_broker, mqtt_port);
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
    mqttClient.publish(mqtt_topic, String(distance).c_str());
  }

  delay(5000);  
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
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
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float time = pulseIn(echoPin, HIGH);
  float distance = time * 0.0343 / 2;
  return distance;
}
