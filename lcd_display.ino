#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "privatered";
const char* password = "vfpk0135";
const char* mqtt_broker = "192.168.11.213";
const int mqtt_port = 1883;
const char* mqtt_topic = "sensor/distance";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
String ipMessage;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  connectToWiFi();
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  lcdPrint("Distance: " + message + " cm");
}

void connectToWiFi() {
  lcdPrint("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcdPrint("Connecting to WiFi.");
  }
  Serial.println("\nConnected to WiFi");
  ipMessage = "IP: " + WiFi.localIP().toString();
  Serial.println(ipMessage);
  lcdPrint(ipMessage);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    lcdPrint("Connecting to MQTT");
    lcdPrint(ipMessage); 
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
      lcdPrint("MQTT Connected");
      mqttClient.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      lcdPrint("MQTT Connect failed");
      delay(5000);
    }
  }
}

void lcdPrint(String message) {
  lcd.clear();
  lcd.print(message);
  delay(500); 
}
