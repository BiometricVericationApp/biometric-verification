#ifndef RPI_H_
#define RPI_H_

#include "common/lock.h"
#include "timeout.h"

SemaphoreHandle_t nameSemaphore;

String name;

void setUpRpiIntegration() {
    nameSemaphore = xSemaphoreCreateMutex();
}


void updateName(String newName) {
  WITH_SEMAPHORE(nameSemaphore, {
    name = newName;
    updateAction(Name);
  });
}

String getName() {
  String retName;
  WITH_SEMAPHORE(bpmSemaphore, {
    retName = name;
  });
  return retName;
}


#endif // RPI_H_
