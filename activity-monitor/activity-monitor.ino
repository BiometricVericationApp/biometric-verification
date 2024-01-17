#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "timeout.h"
#include "integrations/user-data-collector.h"
#include "integrations/presence.h"
#include "integrations/rpi.h"
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
void executeCurrentAction(void *pvParameters);

void setup() {
  Serial.begin(115200);

  /* SetUp Hardware Components */
  setUpLcd();
  setUpLeds();
  setUpWifi();
  
  /* SetUp Timeouts for showing the "No Data" screen */
  setUpAction();
  setUpTimeout();

  // Set Up integrations (data structures for each integration)
  setUpDataCollector();
  setUpRpiIntegration();
  setUpPresence();

  // SetUp MQTT Receivers
  xTaskCreate(MQTTTask, "MQTT Task", 10000, NULL, 4, NULL);
  xTaskCreate(executeCurrentAction, "Execute a current action", 10000, NULL, 6, NULL);
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
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Client")) {
      mqttClient.subscribe(TOPIC_DISTANCE_SENSOR_1);
      mqttClient.subscribe(TOPIC_DISTANCE_SENSOR_2);
      mqttClient.subscribe(TOPIC_HEART);
      mqttClient.subscribe(TOPIC_GALVANIC);
      mqttClient.subscribe(TOPIC_RPI);
      lcdPrint("MQTT Connected");
    } else {
      lcdPrint("MQTT ERR:" + String(mqttClient.state()));
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
  } else if (String(topic) == TOPIC_HEART) {
    float bpm = message.toFloat();
    updateBpm(bpm);
  } else if (String(topic) == TOPIC_GALVANIC) {
    float gsr = message.toFloat();
    updateGsr(gsr);
  } else if (String(topic) == TOPIC_RPI) {
    updateName(message);
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
    } else if (action == None) {
        int x = getNumberOfNoPackages();
        lcdPrint("No Data: " + String(x));
        turnOffLeds();
    } else if (action == Name) {
        String name = getName();
        lcdPrint("Is: " + name);
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
