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
 return RESOURCE_FROM_SEMAPHORE(nameSemaphore, String, name);
}


#endif // RPI_H_
