#ifndef GLOBAL_DATA_
#define GLOBAL_DATA_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "presence.h"
#include "lock.h"

enum LastAction {
    Heart,
    Distance,
    None,
};

int counter = 0;
LastAction lastAction = None;

SemaphoreHandle_t actionSemaphore;
SemaphoreHandle_t counterSemaphore;

void setUpGlobalData() {
    actionSemaphore = xSemaphoreCreateMutex();
    counterSemaphore = xSemaphoreCreateMutex();
}

int getNumberOfNoPackages() {
    int c;
    WITH_SEMAPHORE(counterSemaphore,{
      c = counter;
    });
    return c;
}

void updateNumberOfNoPackages() {
  WITH_SEMAPHORE(counterSemaphore, {
    counter += 1;
  });
}

void resetNumberOfPackages() {
  WITH_SEMAPHORE(counterSemaphore, {
    counter = 0;
  });
}

/*
 * Action
 */

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

#endif // GLOBAL_DATA_
