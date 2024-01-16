#ifndef USER_DATA_COLLECTOR_
#define USER_DATA_COLLECTOR_

#include "lock.h"
#include "timeout.h"

SemaphoreHandle_t bpmSemaphore;
SemaphoreHandle_t gsrSemaphore;

float actual_bpm;
float actual_gsr;

void setUpDataCollector() {
    bpmSemaphore = xSemaphoreCreateMutex();
    gsrSemaphore = xSemaphoreCreateMutex();
}


void updateBpm(float bpm) {
  WITH_SEMAPHORE(bpmSemaphore, {
    actual_bpm = bpm;
    updateAction(Heart);
  });
}

float getBpm() {
  float bpm;
  WITH_SEMAPHORE(bpmSemaphore, {
    bpm = actual_bpm;
  });
  return bpm;
}

void updateGsr(float gsr) {
  WITH_SEMAPHORE(gsrSemaphore, {
    actual_gsr = gsr;
    updateAction(Heart);
  });
}

float getGsr() {
  float gsr;
  WITH_SEMAPHORE(gsrSemaphore, {
    gsr = actual_gsr;
  });
  return gsr;
}

#endif // USER_DATA_COLLECTOR_
