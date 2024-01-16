/*
 * DETECTION_RANGE => Range of Detection
 * TOLERANCE => Error marge for considering an object in center
*/
#ifndef PRESENCE_H_   /* Include guard */
#define PRESENCE_H_

#include <Arduino.h>
#include "lock.h"
#include "global-data.h"

#define DETECTION_RANGE 50.0
#define TOLERANCE 5.0

#define CENTER_STR "Center"
#define LEFT_STR "Left"
#define RIGHT_STR "Right"

struct Distance {
    float leftDistance;
    float rightDistance;
};

struct DistanceResult {
    bool hasData;
    String direction;
    float proximity;
};

struct DistanceInfo {
    struct Distance current;
    DistanceResult last;
};

bool inRange(float dist) {
    return dist > 0.0 && dist <= DETECTION_RANGE;
}

DistanceResult checkForPresenceAndDirection(DistanceInfo info) {
    float leftDistance = info.current.leftDistance;
    float rightDistance = info.current.rightDistance;
    float proximity;
    bool isLeft = false, isRight = false, isCenter = false;
    if (inRange(leftDistance) || inRange(rightDistance)) {
        if (inRange(leftDistance) && inRange(rightDistance)) {
            isCenter = abs(rightDistance - leftDistance) <= TOLERANCE;
            isLeft = !isCenter && rightDistance < leftDistance;
            isRight = !isCenter && leftDistance < rightDistance;
            proximity = (rightDistance + leftDistance) / 2;
        } else if (inRange(leftDistance)){
            proximity = leftDistance;
            isLeft = true;
        } else {
            proximity = rightDistance;
            isRight = true;
        }

        String direction = isCenter ? CENTER_STR : (isLeft ? LEFT_STR : RIGHT_STR);
        return {.hasData = true, .direction = direction, .proximity = proximity};
    }
    return {.hasData = false};
}

/*
 * Distance safe 
 */

SemaphoreHandle_t leftDistanceSemaphore;
SemaphoreHandle_t rightDistanceSemaphore;
SemaphoreHandle_t lastDistanceSemaphore;

struct DistanceInfo distance;

void setUpPresence() {
    leftDistanceSemaphore = xSemaphoreCreateMutex();
    rightDistanceSemaphore = xSemaphoreCreateMutex();
    lastDistanceSemaphore = xSemaphoreCreateMutex();
}


void updateLeftDistance(float newDistance) {
  WITH_SEMAPHORE(leftDistanceSemaphore, {
      distance.current.leftDistance = newDistance;
  });
}

void updateRightDistance(float newDistance) {
  WITH_SEMAPHORE(rightDistanceSemaphore, {
      distance.current.rightDistance = newDistance;
  });
}


void updateLastDistance(DistanceResult result) {
  WITH_SEMAPHORE(lastDistanceSemaphore, {
      distance.last = result;
      updateAction(Distance);
  });
}

struct DistanceInfo getDistance() {
    struct DistanceInfo dist;
    WITH_SEMAPHORE(rightDistanceSemaphore, {
        WITH_SEMAPHORE(leftDistanceSemaphore, {
            memcpy(&dist, &(distance), sizeof(DistanceInfo));
        });
    });
    return dist;
}

#endif // PRESENCE_H_
