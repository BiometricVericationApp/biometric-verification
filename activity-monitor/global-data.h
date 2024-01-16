#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "presence.h"
#include "lock.h"

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
    info.lastAction = action;
    resetNumberOfPackages();
  });
}

LastAction getLastAction() {
  LastAction action;
  WITH_SEMAPHORE(actionSemaphore, {
    action = info.lastAction;
  });
  return action;
}

/*
 * Distances
 */


void updateLeftDistance(float newDistance) {
  WITH_SEMAPHORE(leftDistanceSemaphore, {
      info.distance.current.leftDistance = newDistance;
  });
}

void updateRightDistance(float newDistance) {
  WITH_SEMAPHORE(rightDistanceSemaphore, {
      info.distance.current.rightDistance = newDistance;
  });
}


void updateLastDistance(DistanceResult result) {
  WITH_SEMAPHORE(lastDistanceSemaphore, {
      info.distance.last = result;
      updateAction(Distance);
  });
}

struct DistanceInfo getDistance() {
    struct DistanceInfo dist;
    WITH_SEMAPHORE(rightDistanceSemaphore, {
        WITH_SEMAPHORE(leftDistanceSemaphore, {
            memcpy(&dist, &(info.distance), sizeof(DistanceInfo));
        });
    });
    return dist;
}


/*
 * BPM
 */

void updateBpm(float bpm) {
  WITH_SEMAPHORE(bpmSemaphore, {
      info.heart.bpm = bpm;
      updateAction(Heart);
  });
}

float getBpm() {
  float bpm;
  WITH_SEMAPHORE(bpmSemaphore, {
      bpm = info.heart.bpm;
  });
  return bpm;
}

/*
 * GSR
 */

void updateGsr(float gsr) {
  WITH_SEMAPHORE(gsrSemaphore, {
      info.heart.gsr = gsr;
      updateAction(Heart);
  });
}

float getGsr() {
  float gsr;
  WITH_SEMAPHORE(gsrSemaphore, {
      gsr = info.heart.gsr;
  });
  return gsr;
}
