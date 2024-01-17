#ifndef WIFI_H_
#define WIFI_H_
#include "../common/common.h"
#include "lcd.h"

/*
 *library for connecting to wifi (and retrial).
 *NOTE: be aware that prints to the lcd screen, so initiate the screen first!
 */
#define MILISECONDS_CHECK_WIFI 10000
#define MILISECONDS_RETRY_WIFI 500

void WiFiTask(void *pvParameters)
{
	for (;;)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			WiFi.begin(SSID_WIFI, SSID_PASSWORD);
			for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
			{
				vTaskDelay(MILISECONDS_RETRY_WIFI / portTICK_PERIOD_MS);
				lcdPrint("Wifi, r:" + String(i));
			}
		}

		vTaskDelay(MILISECONDS_CHECK_WIFI / portTICK_PERIOD_MS);
	}
}

void setUpWifi()
{
	xTaskCreate(WiFiTask, "WiFi Task", 10000, NULL, 1, NULL);
}


#endif	// WIFI_H_
