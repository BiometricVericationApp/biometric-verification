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

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void MQTTTask(void *pvParameters);
void reconnectMQTT();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void showCurrentAction(void *pvParameters);

void setup()
{
	Serial.begin(115200);

	/*SetUp Hardware Components */
	setUpLcd();
	setUpLeds();
	setUpWifi();

	/*SetUp Timeouts for showing the "No Data" screen */
	setUpTimeout();

	// Set Up integrations (data structures for each integration)
	setUpDataCollector();
	setUpRpiIntegration();
	setUpPresence();

	// SetUp MQTT Receivers
	xTaskCreate(MQTTTask, "MQTT Task", 10000, NULL, 4, NULL);
	xTaskCreate(showCurrentAction, "Execute a current action", 10000, NULL, 6, NULL);
}

void loop() {}

void MQTTTask(void *pvParameters)
{
	mqttClient.setServer(IP_MQTT_BROKER, PORT_MQTT_BROKER);
	mqttClient.setCallback(mqttCallback);

	for (;;)
	{
		if (!mqttClient.connected())
		{
			reconnectMQTT();
		}

		mqttClient.loop();
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void reconnectMQTT()
{
	while (!mqttClient.connected())
	{
		Serial.print("Attempting MQTT connection...");
		if (mqttClient.connect("ESP32Client"))
		{
			mqttClient.subscribe(TOPIC_DISTANCE_SENSOR_1);
			mqttClient.subscribe(TOPIC_DISTANCE_SENSOR_2);
			mqttClient.subscribe(TOPIC_HEART);
			mqttClient.subscribe(TOPIC_GALVANIC);
			mqttClient.subscribe(TOPIC_RPI);
			lcdPrint("MQTT Connected");
		}
		else
		{
			lcdPrint("MQTT ERR:" + String(mqttClient.state()));
			delay(5000);
		}
	}
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
	String message;
	for (unsigned int i = 0; i < length; i++)
	{
		message += (char) payload[i];
	}

	executeActionFromTopic(String(topic), message);
}

void executeActionFromTopic(String topic, String message)
{
	if (topic == TOPIC_DISTANCE_SENSOR_1)
	{
		float leftDist = message.toFloat();
		updateLeftDistance(leftDist);
		updateLastDistance();
	}
	else if (topic == TOPIC_DISTANCE_SENSOR_2)
	{
		float rightDist = message.toFloat();
		updateRightDistance(rightDist);
		updateLastDistance();
	}
	else if (topic == TOPIC_HEART)
	{
		float bpm = message.toFloat();
		updateBpm(bpm);
	}
	else if (topic == TOPIC_GALVANIC)
	{
		float gsr = message.toFloat();
		updateGsr(gsr);
	}
	else if (topic == TOPIC_RPI)
	{
		updateName(message);
	}
}

void updateLastDistance()
{
	struct DistanceInfo dist = getDistance();
	DistanceResult result = checkForPresenceAndDirection(dist);
	if (result.hasData)
	{
		updateLastDistance(result);
	}
}

void showCurrentAction(void *pvParameters)
{
	for (;;)
	{
		LastAction action = getLastAction();
		if (action == Heart)
		{
			showActionHeart();
		}
		else if (action == Distance)
		{
			showActionDistance();
		}
		else if (action == None)
		{
			showNoData();
		}
		else if (action == Name)
		{
			String name = getName();
			lcdPrint("Is: " + name);
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void showActionHeart()
{
	lcdPrint("BPM: " + String(getBpm()), Up);
	lcdPrint("GSR: " + String(getGsr()), Down);
}

void showActionDistance()
{
	struct DistanceInfo dist = getDistance();
	if (dist.last.hasData)
	{
		lcdPrint(dist.last.direction, Up);
		lcdPrint("CM: " + String(dist.last.proximity), Down);
		printDistanceInLeds(dist.last.proximity);
	}
}

void showNoData()
{
	int x = getNumberOfNoPackages();
	lcdPrint("No Data: " + String(x));
	turnOffLeds();
}

void printDistanceInLeds(float proximity)
{
	int ledsToTurnOn = map(proximity, 0, DETECTION_RANGE, NUM_LEDS, 0);
	turnOnLeds(ledsToTurnOn);
}
