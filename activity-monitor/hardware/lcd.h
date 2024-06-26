#ifndef LCD_H_
#define LCD_H_
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/semphr.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../common/lock.h"

/*
 * library for managing the lcd display without racing conditions
 */

LiquidCrystal_I2C lcd(0x27, 16, 2);
SemaphoreHandle_t xSemaphoreLCD;

void setUpLcd()
{
	lcd.init();
	lcd.backlight();
	xSemaphoreLCD = xSemaphoreCreateMutex();
}

enum ScreenLine
{
	Up,
	Down
};

void lcdPrint(String message, ScreenLine line)
{
	WITH_SEMAPHORE(xSemaphoreLCD,
	{
		lcd.setCursor(0, line);
		lcd.print(message);
		for (int i = message.length(); i < 16; i++)
		{
			lcd.print(" ");
		} 
    });
}

void lcdPrint(String message)
{
	lcdPrint(message, Up);
	lcdPrint("", Down);
}


#endif	// LCD_H_
