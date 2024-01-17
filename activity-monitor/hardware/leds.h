#ifndef LEDS_H_
#define LEDS_H_

#include <Arduino.h>
/*
 * name: leds.h
 * description: file for managing the leds.
*/

#define NUM_LEDS 10
const int LED_PINS[] = {12, 14, 27, 26, 33, 32, 36, 34, 16, 17}; 

void setUpLeds() {
  for(int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
}

void turnOnLeds(int numLedsToTurnOn) {
  numLedsToTurnOn = constrain(numLedsToTurnOn, 0, NUM_LEDS);
  for(int i = 0; i < NUM_LEDS; i++) {
    if (i < numLedsToTurnOn) {
      digitalWrite(LED_PINS[i], HIGH);
    } else {
      digitalWrite(LED_PINS[i], LOW);
    }
  }
}

void turnOffLeds() {
  for(int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
}

#endif // LEDS_H_
