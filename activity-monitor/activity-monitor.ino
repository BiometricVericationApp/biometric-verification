#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "presence.h"
#include "timeout.h"
#include "user-data-collector.h"
#include "hardware/wifi.h"
#include "hardware/leds.h"
#include "hardware/lcd.h"
#include "common/common.h"
#include "common/common_ultrasound.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
void MQTTTask(void *pvParameters);
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void lcdPrint(String message, int line);
void executeCurrentAction(void *pvParameters);

void setup() {
  Serial.begin(115200);

  /* SetUp Hardware Components */
  setUpWifi();
  setUpLcd();
  setUpLeds();
  
  /* SetUp Software (for locks, initial vars, etc) */
  setUpAction();
  setUpTimeout();
  setUpDataCollector();
  setUpPresence();
  xTaskCreate(MQTTTask, "MQTT Task", 10000, NULL, 1, NULL);
  xTaskCreate(executeCurrentAction, "Execute a current action", 10000, NULL, 1, NULL);
}


void loop() {}

/*
    MQTT
*/

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
  executeActionFromTopic(topic, message);
}

void executeActionFromTopic(char *topic, String message) {
  if (String(topic) == TOPIC_DISTANCE_SENSOR_1) {
    float leftDist = message.toFloat();
    updateLeftDistance(leftDist);
    updateA();
  } else if (String(topic) == TOPIC_DISTANCE_SENSOR_2) {
    float rightDist = message.toFloat();
    updateRightDistance(rightDist);
    updateA();
  } else if (String(topic) == TOPIC_SENSOR_3_DATA) {
    if (message.startsWith("Sensor Gsr: ")) {
      float gsr = message.substring(12).toFloat();
      updateGsr(gsr);
    } else if (message.startsWith("Bpm: ")) {
      float bpm = message.substring(5).toFloat();
      updateBpm(bpm);
    }
  }
}


void updateA() {
    struct DistanceInfo dist = getDistance();
    DistanceResult result = checkForPresenceAndDirection(dist);
    if (result.hasData) {
        updateLastDistance(result);
    }
}


void executeCurrentAction(void *pvParameters) {
  for (;;) {
    LastAction action = getLastAction();
    if (action == Heart) {
        executeActionHeart();
    } else if (action == Distance) {
        executeActionDistance();
    } else {
        lcdPrint("Nothing Received...");
        turnOffLeds();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void executeActionHeart() {
    lcdPrint("BPM: " + String(getBpm()), Up);
    lcdPrint("GSR: " + String(getGsr()), Down); 
}

void executeActionDistance() {
    struct DistanceInfo dist = getDistance();
    if (dist.last.hasData) {
        lcdPrint(dist.last.direction, Up);
        lcdPrint("CM: " + String(dist.last.proximity), Down);
        printDistanceInLeds(dist.last.proximity);
    }
}

void printDistanceInLeds(float proximity) {
    int ledsToTurnOn = map(proximity, 0, DETECTION_RANGE, NUM_LEDS, 0);
    turnOnLeds(ledsToTurnOn);
}
