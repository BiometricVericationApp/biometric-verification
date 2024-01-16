#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "common/lock.h"


/*
 * file: timeout.h
 * description: saves and tells where to timeout an action (this means, 
 * when should we put nothing received on screen)
 */


#define THRESHOLD 4


enum LastAction {
    Heart,
    Distance,
    None,
};
int counter = 0;
LastAction lastAction = None;

SemaphoreHandle_t actionSemaphore;
SemaphoreHandle_t counterSemaphore;


static void resetNumberOfPackages() {
  WITH_SEMAPHORE(counterSemaphore, {
    counter = 0;
  });
}

static int getNumberOfNoPackages() {
    int c;
    WITH_SEMAPHORE(counterSemaphore,{
      c = counter;
    });
    return c;
}

static void updateNumberOfNoPackages() {
  WITH_SEMAPHORE(counterSemaphore, {
    counter += 1;
  });
}


void setUpAction() {
    actionSemaphore = xSemaphoreCreateMutex();
}

void updateAction(LastAction action) {
  WITH_SEMAPHORE(actionSemaphore, {
    lastAction = action;
    resetNumberOfPackages();
  });
}

LastAction getLastAction() {
  LastAction action;
  WITH_SEMAPHORE(actionSemaphore, {
    action = lastAction;
  });
  return action;
}

void markAsNonUpdated(void *pvParameters) {
  for (;;) {
    int counter = getNumberOfNoPackages();
    if (counter >= THRESHOLD) {
        updateAction(None);
    }
    updateNumberOfNoPackages();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setUpTimeout() {
    counterSemaphore = xSemaphoreCreateMutex();
    xTaskCreate(markAsNonUpdated, "Mark as None", 10000, NULL, 1, NULL);
}

#endif // TIMEOUT_H_
