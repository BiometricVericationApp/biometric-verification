#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Network credentials
const char* ssid = "privatered";
const char* password = "vfpk0135";

// MQTT Broker settings
const char* mqtt_broker = "192.168.249.213";
const int mqtt_port = 1883;
const char* mqtt_topic1 = "sensor1/distance"; 
const char* mqtt_topic2 = "sensor2/distance"; 

WiFiClient espClient;
PubSubClient mqttClient(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

SemaphoreHandle_t xSemaphoreLCD; // Semáforo para el LCD

float distance1 = 0.0;
float distance2 = 0.0;
const float detectionRange = 100.0; // Distancia de detección en cm

void WiFiTask(void *pvParameters);
void MQTTTask(void *pvParameters);
void LCDDisplayTask(void *pvParameters);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void checkForPresenceAndDirection(float d1, float d2, float range);
void reconnectMQTT();
void lcdPrint(String message, int line);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  xSemaphoreLCD = xSemaphoreCreateMutex(); // Crear semáforo para el LCD

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
     // lcdPrint("Connecting to WiFi",1);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      Serial.println("\nConnected to WiFi");
   //   lcdPrint("IP: " + WiFi.localIP().toString(),2);
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
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
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void LCDDisplayTask(void *pvParameters) {
  for (;;) {
    if (distance1 > 0 && distance2 > 0) {
      checkForPresenceAndDirection(distance1, distance2, detectionRange);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
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
    if (d1 <= range && d2 <= range) {
        float sensorDistance = 30.0; // en cm
        float angleBetweenSensors = 45.0; // en grados

        float cosC = (d1 * d1 + d2 * d2 - sensorDistance * sensorDistance) / (2 * d1 * d2);

        // Comprobar que cosC está en el rango válido para el arcocoseno
        if (cosC > 1.0) cosC = 1.0;
        if (cosC < -1.0) cosC = -1.0;

        float angleC = acos(cosC);

        float leftPercentage = 0.0;
        float rightPercentage = 0.0;

        if (angleC < PI / 2) {
            leftPercentage = 100.0 * (1 - angleC / (PI / 2));
        } else {
            rightPercentage = 100.0 * (1 - (PI - angleC) / (PI / 2));
        }

        String line1 = "Left: " + String(leftPercentage, 2) + "%";
        String line2 = "Right: " + String(rightPercentage, 2) + "%";

        lcdPrint(line1, 1);
        lcdPrint(line2, 2);
    } else {
        lcdPrint("Fuera de rango", 1);
        lcdPrint("", 2);
    }
}


void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
   // lcdPrint("Connecting to MQTT",1);
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
    //  lcdPrint("MQTT Connected",2);
      mqttClient.subscribe(mqtt_topic1);
      mqttClient.subscribe(mqtt_topic2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
    //  lcdPrint("MQTT Connect failed",2);
      delay(5000);
    }
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
    // Reducir o eliminar este delay según sea necesario
    delay(100); // Ajusta este valor según tus necesidades
  }
}



