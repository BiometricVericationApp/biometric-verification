#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "lock.h"

/*
 * file: timeout.h
 * description: saves and tells where to timeout an action (this means, 
 * when should we put nothing received on screen)
 */

int counter = 0;

SemaphoreHandle_t counterSemaphore;

void setUpTimeout() {
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

#endif // TIMEOUT_H_
