#ifndef WIFI_H_
#define WIFI_H_

#include "common.h"

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


void setUpWifi() {
  xTaskCreate(WiFiTask, "WiFi Task", 10000, NULL, 1, NULL);
}

#endif // WIFI_H_
