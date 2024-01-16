/*
 * DETECTION_RANGE => Range of Detection
 * TOLERANCE => Error marge for considering an object in center
*/
#ifndef PRESENCE_H_   /* Include guard */
#define PRESENCE_H_

#include <Arduino.h>

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
    Distance current;
    DistanceResult last;
};

bool inRange(float distance) {
    return distance > 0.0 && distance <= DETECTION_RANGE;
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

#endif // PRESENCE_H_
