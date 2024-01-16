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
#include "lcd.h"

#define INCLUDE_vTaskSuspend  1

WiFiClient espClient;
PubSubClient mqttClient(espClient);

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
void updateDevicesTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  setUpLcd();
  setUpLeds();
  xSemaphoreInfo = xSemaphoreCreateMutex(); 
  xTaskCreate(WiFiTask, "WiFi Task", 10000, NULL, 1, NULL);
  xTaskCreate(MQTTTask, "MQTT Task", 10000, NULL, 1, NULL);
  xTaskCreate(updateDevicesTask, "Update Devices Task", 10000, NULL, 1, NULL);
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

void updateDevicesTask(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(xSemaphoreInfo, portMAX_DELAY)) {
        if (info.lastAction == Heart) {
            executeActionHeart();
        } else if (info.lastAction == Distance) {
            executeActionDistance();
        } else {
          lcdPrint("Nothing Received...");
        }
        xSemaphoreGive(xSemaphoreInfo);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void executeActionHeart() {
    lcdPrint("BPM: " + String(info.heart.bpm), Up);
    lcdPrint("GSR: " + String(info.heart.gsr), Down); 
}

void executeActionDistance() {
    DistanceResult result = checkForPresenceAndDirection(info.distance);
    info.distance.last = result;
    if (result.hasData) {
        lcdPrint(result.direction, Up);
        lcdPrint("CM: " + String(result.proximity), Down);
        printDistanceInLeds(result.proximity);
    }
}

void printDistanceInLeds(float proximity) {
    int ledsToTurnOn = map(proximity, 0, DETECTION_RANGE, NUM_LEDS, 0);
    turnOnLeds(ledsToTurnOn);
}

