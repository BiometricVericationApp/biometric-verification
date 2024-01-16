#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <freertos/semphr.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../common/lock.h"

/*
 * name: lcd.h
 * description: file for managing the lcd without racing conditions
*/

LiquidCrystal_I2C lcd(0x27, 16, 2); 
SemaphoreHandle_t xSemaphoreLCD;

void setUpLcd() {
  lcd.init();
  lcd.backlight();
  xSemaphoreLCD = xSemaphoreCreateMutex(); 
}

enum ScreenLine {
    Up,
    Down
};

void lcdPrint(String message, ScreenLine line) {
  WITH_SEMAPHORE(xSemaphoreLCD, {
    lcd.setCursor(0, line);
    lcd.print(message);
    for (int i = message.length(); i < 16; i++) {
      lcd.print(" ");
    }
  });
  delay(100); 
}

void lcdPrint(String message) {
    lcdPrint(message, Up);
    lcdPrint("", Down);
}
