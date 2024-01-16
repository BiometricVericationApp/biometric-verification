#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "presence.h"

#define INCLUDE_vTaskSuspend  1

enum LastAction {
    Heart,
    Distance,
    None,
};

struct HeartInfo {
    float gsr;
    float bpm;
};

struct GlobalInfo {
    HeartInfo heart;
    DistanceInfo distance;
    LastAction lastAction;
};

int counter = 0;
GlobalInfo info = { .lastAction = None };

SemaphoreHandle_t actionSemaphore;
SemaphoreHandle_t leftDistanceSemaphore;
SemaphoreHandle_t rightDistanceSemaphore;
SemaphoreHandle_t bpmSemaphore;
SemaphoreHandle_t gsrSemaphore;
SemaphoreHandle_t lastDistanceSemaphore;
SemaphoreHandle_t counterSemaphore;

void setUpGlobalData() {
    actionSemaphore = xSemaphoreCreateMutex();
    leftDistanceSemaphore = xSemaphoreCreateMutex();
    rightDistanceSemaphore = xSemaphoreCreateMutex();
    bpmSemaphore = xSemaphoreCreateMutex();
    gsrSemaphore = xSemaphoreCreateMutex();
    lastDistanceSemaphore = xSemaphoreCreateMutex();
    counterSemaphore = xSemaphoreCreateMutex();
}

int getNumberOfNoPackages() {
  int c;
  if (xSemaphoreTake(counterSemaphore, portMAX_DELAY)) {
      c = counter;
  }
  xSemaphoreGive(counterSemaphore);
  return c;
}

void updateNumberOfNoPackages() {
  if (xSemaphoreTake(counterSemaphore, portMAX_DELAY)) {
      counter += 1;
  }
  xSemaphoreGive(counterSemaphore);
}

void resetNumberOfPackages() {
  if (xSemaphoreTake(counterSemaphore, portMAX_DELAY)) {
      counter = 0;
  }
  xSemaphoreGive(counterSemaphore);
}

/*
 * Action
 */

void updateAction(LastAction action) {
  if (xSemaphoreTake(actionSemaphore, portMAX_DELAY)) {
      info.lastAction = action;
      resetNumberOfPackages();
  }
  xSemaphoreGive(actionSemaphore);
}

LastAction getLastAction() {
  LastAction action;
  if (xSemaphoreTake(actionSemaphore, portMAX_DELAY)) {
      action = info.lastAction;
  }
  xSemaphoreGive(actionSemaphore);
  return action;
}

/*
 * Distances
 */


void updateLeftDistance(float newDistance) {
  if (xSemaphoreTake(leftDistanceSemaphore, portMAX_DELAY)) {
      info.distance.current.leftDistance = newDistance;
  }
  xSemaphoreGive(leftDistanceSemaphore);
}

void updateRightDistance(float newDistance) {
  if (xSemaphoreTake(rightDistanceSemaphore, portMAX_DELAY)) {
      info.distance.current.rightDistance = newDistance;
  }
  xSemaphoreGive(rightDistanceSemaphore);
}


void updateLastDistance(DistanceResult result) {
  if (xSemaphoreTake(lastDistanceSemaphore, portMAX_DELAY)) {
      info.distance.last = result;
      updateAction(Distance);
  }
  xSemaphoreGive(lastDistanceSemaphore);
}

struct DistanceInfo getDistance() {
    struct DistanceInfo dist;
    if (xSemaphoreTake(rightDistanceSemaphore, portMAX_DELAY) && xSemaphoreTake(leftDistanceSemaphore, portMAX_DELAY)) {
        memcpy(&dist, &(info.distance), sizeof(DistanceInfo));
    }
    xSemaphoreGive(leftDistanceSemaphore);
    xSemaphoreGive(rightDistanceSemaphore);
    return dist;
}


/*
 * BPM
 */

void updateBpm(float bpm) {
  if (xSemaphoreTake(bpmSemaphore, portMAX_DELAY)) {
      info.heart.bpm = bpm;
      updateAction(Heart);
  }
  xSemaphoreGive(bpmSemaphore);
}

float getBpm() {
  float bpm;
  if (xSemaphoreTake(bpmSemaphore, portMAX_DELAY)) {
      bpm = info.heart.bpm;
  }
  xSemaphoreGive(bpmSemaphore);
  return bpm;
}

/*
 * GSR
 */

void updateGsr(float gsr) {
  if (xSemaphoreTake(gsrSemaphore, portMAX_DELAY)) {
      info.heart.gsr = gsr;
      updateAction(Heart);
  }
  xSemaphoreGive(gsrSemaphore);
}

float getGsr() {
  float gsr;
  if (xSemaphoreTake(gsrSemaphore, portMAX_DELAY)) {
      gsr = info.heart.gsr;
  }
  xSemaphoreGive(gsrSemaphore);
  return gsr;
}
