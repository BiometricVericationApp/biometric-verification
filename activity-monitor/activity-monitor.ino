#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "common.h"
#include "common_ultrasound.h"
#include "leds.h"
#include "presence.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

SemaphoreHandle_t xSemaphoreLCD;

enum LastAction {
    Heart,
    Distance,
    None,
};

struct HeartInfo {
    float gsr;
    float bpm;
};

struct GlobalInfo {
    HeartInfo heart;
    DistanceInfo distance;
    LastAction lastAction;
};

GlobalInfo info = { .lastAction = None };
SemaphoreHandle_t xSemaphoreInfo;

void WiFiTask(void *pvParameters);
void MQTTTask(void *pvParameters);
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void lcdPrint(String message, int line);
void LCDDisplayTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  xSemaphoreLCD = xSemaphoreCreateMutex(); 
  xSemaphoreInfo = xSemaphoreCreateMutex(); 
  xTaskCreate(WiFiTask, "WiFi Task", 10000, NULL, 1, NULL);
  xTaskCreate(MQTTTask, "MQTT Task", 10000, NULL, 1, NULL);
  xTaskCreate(LCDDisplayTask, "LCD Display Task", 10000, NULL, 1, NULL);
  setUpLeds();
}


void loop() {}

/*
    MQTT and wifi related
*/

void WiFiTask(void *pvParameters) {
  for (;;) {
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(SSID_WIFI, SSID_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      Serial.println("\nConnected to WiFi");
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}


void MQTTTask(void *pvParameters) {
  mqttClient.setServer(IP_MQTT_BROKER, PORT_MQTT_BROKER);
  mqttClient.setCallback(mqttCallback);

  for (;;) {
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
      mqttClient.subscribe(TOPIC_DISTANCE_SENSOR_1);
      mqttClient.subscribe(TOPIC_DISTANCE_SENSOR_2);
      mqttClient.subscribe(TOPIC_SENSOR_3_DATA);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (xSemaphoreTake(xSemaphoreInfo, portMAX_DELAY)) {
    if (String(topic) == TOPIC_DISTANCE_SENSOR_1) {
      info.distance.current.leftDistance = message.toFloat();
      info.lastAction = Distance;
    } else if (String(topic) == TOPIC_DISTANCE_SENSOR_2) {
      info.distance.current.rightDistance = message.toFloat();
      info.lastAction = Distance;
    } else if (String(topic) == TOPIC_SENSOR_3_DATA) {
      if (message.startsWith("Sensor Gsr: ")) {
        info.heart.gsr = message.substring(12).toFloat();
      } else if (message.startsWith("Bpm: ")) {
        info.heart.bpm = message.substring(5).toFloat();
      }
      info.lastAction = Heart;
    }
    xSemaphoreGive(xSemaphoreInfo);
  }
}

/*
    LCD Display 
*/

void LCDDisplayTask(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(xSemaphoreInfo, portMAX_DELAY)) {
        if (info.lastAction == Heart) {
          lcdPrint("BPM: " + String(info.heart.bpm),1);
          lcdPrint("GSR: " + String(info.heart.gsr), 2); 
          info.lastAction = Distance;
        } else if (info.lastAction == Distance) {
          DistanceResult result = checkForPresenceAndDirection(info.distance);
          info.distance.last = result;
          if (result.hasData) {
            lcdPrint(result.direction, 1);
            lcdPrint("CM: " + String(result.proximity), 2);
          }
        } else {
          lcdPrint("Nothing Received...", 1);
        }
        xSemaphoreGive(xSemaphoreInfo);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}


void lcdPrint(String message, int line) {
  if (xSemaphoreTake(xSemaphoreLCD, portMAX_DELAY)) {
    lcd.setCursor(0, line - 1);
    lcd.print(message);
    for (int i = message.length(); i < 16; i++) {
      lcd.print(" ");
    }
    xSemaphoreGive(xSemaphoreLCD);
    delay(100); 
  }
}

