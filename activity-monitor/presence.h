/*
 * DETECTION_RANGE => Range of Detection
 * TOLERANCE => Error marge for considering an object in center
 * NOISE_THRESHOLD => Ignore changes that changed below this threshold
*/
#include <Arduino.h>

#define DETECTION_RANGE 50.0
#define TOLERANCE 5.0
#define NOISE_THRESHOLD 2.0

#define CENTER_STR "Center"
#define LEFT_STR "Left"
#define RIGHT_STR "Right"
#define NONE_STR "None"

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


DistanceResult checkForPresenceAndDirection(DistanceInfo info) {
    float leftDistance = info.current.leftDistance;
    float rightDistance = info.current.rightDistance;
    float proximity;
    bool isLeft = false, isRight = false, isCenter = false;
    if (rightDistance <= DETECTION_RANGE || leftDistance <= DETECTION_RANGE) {
        if (rightDistance <= DETECTION_RANGE && leftDistance <= DETECTION_RANGE) {
            isCenter = abs(rightDistance - leftDistance) <= TOLERANCE;
            isLeft = !isCenter && rightDistance < leftDistance;
            isRight = !isCenter && leftDistance < rightDistance;
            proximity = (rightDistance + leftDistance) / 2;
        } else if (leftDistance <= DETECTION_RANGE){
            proximity = leftDistance;
            isLeft = true;
        } else {
            proximity = rightDistance;
            isRight = true;
        }

        if (abs(proximity - info.last.proximity) <= NOISE_THRESHOLD) {
            proximity = info.last.proximity;
        }

        String direction = isCenter ? CENTER_STR : (isLeft ? LEFT_STR : RIGHT_STR);
        return {.hasData = true, .direction = direction, .proximity = proximity};
    }
    return {.hasData = false};
}
