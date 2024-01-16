#ifndef ACTION_H_
#define ACTION_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "lock.h"
#include "timeout.h"

/*
 * file: action.h
 * description: define which action should we print now to the screen
 */

enum LastAction {
    Heart,
    Distance,
    None,
};

LastAction lastAction = None;

SemaphoreHandle_t actionSemaphore;

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

#endif // ACTION_H_
