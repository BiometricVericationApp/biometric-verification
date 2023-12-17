#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Network credentials
const char* ssid = "privatered";
const char* password = "vfpk0135";

// MQTT Broker settings
const char* mqtt_broker = "192.168.11.213";
const int mqtt_port = 1883;
const char* mqtt_topic1 = "sensor1/distance"; 
const char* mqtt_topic2 = "sensor2/distance"; 

WiFiClient espClient;
PubSubClient mqttClient(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

float distance1 = 0.0;
float distance2 = 0.0;
const float detectionRange = 100.0; // Distancia de detección en cm

// Declaración de las funciones de las tareas
void WiFiTask(void *pvParameters);
void MQTTTask(void *pvParameters);
void LCDDisplayTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  // Creación de tareas
  xTaskCreate(WiFiTask, "WiFi Task", 10000, NULL, 1, NULL);
  xTaskCreate(MQTTTask, "MQTT Task", 10000, NULL, 1, NULL);
  xTaskCreate(LCDDisplayTask, "LCD Display Task", 10000, NULL, 1, NULL);
}

void loop() {
  // No se utiliza el loop principal
}

void WiFiTask(void *pvParameters) {
  for (;;) {
    if (WiFi.status() != WL_CONNECTED) {
      lcdPrint("Connecting to WiFi");
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      Serial.println("\nConnected to WiFi");
      lcdPrint("IP: " + WiFi.localIP().toString());
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS); // Revisar la conexión cada 10 segundos
  }
}

void MQTTTask(void *pvParameters) {
  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  for (;;) {
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Loop de MQTT cada segundo
  }
}

void LCDDisplayTask(void *pvParameters) {
  for (;;) {
    if (distance1 > 0 && distance2 > 0) {
      checkForPresenceAndDirection(distance1, distance2, detectionRange);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Actualizar el display cada 2 segundos
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == mqtt_topic1) {
    distance1 = message.toFloat();
  } else if (String(topic) == mqtt_topic2) {
    distance2 = message.toFloat();
  }
}

void checkForPresenceAndDirection(float d1, float d2, float range) {
  if (d1 <= range || d2 <= range) {
    if (d1 < d2) {
      lcdPrint("Persona izquierda");
    } else {
      lcdPrint("Persona derecha");
    }
  } else {
    lcdPrint("No hay persona");
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    lcdPrint("Connecting to MQTT");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
      lcdPrint("MQTT Connected");
      mqttClient.subscribe(mqtt_topic1);
      mqttClient.subscribe(mqtt_topic2);
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
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(2000); 
}
