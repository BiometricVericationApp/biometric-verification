#ifndef USER_DATA_COLLECTOR_
#define USER_DATA_COLLECTOR_

#include "common/lock.h"
#include "timeout.h"

SemaphoreHandle_t bpmSemaphore;
SemaphoreHandle_t gsrSemaphore;

float bpm;
float gsr;

void setUpDataCollector() {
    bpmSemaphore = xSemaphoreCreateMutex();
    gsrSemaphore = xSemaphoreCreateMutex();
}


void updateBpm(float newBpm) {
  WITH_SEMAPHORE(bpmSemaphore, {
    bpm = newBpm;
    updateAction(Heart);
  });
}

float getBpm() {
    return RESOURCE_FROM_SEMAPHORE(bpmSemaphore, float, bpm);
}

void updateGsr(float newGsr) {
  WITH_SEMAPHORE(gsrSemaphore, {
    gsr = newGsr;
    updateAction(Heart);
  });
}

float getGsr() {
    return RESOURCE_FROM_SEMAPHORE(gsrSemaphore, float, gsr);
}

#endif // USER_DATA_COLLECTOR_
