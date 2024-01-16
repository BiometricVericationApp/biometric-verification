#ifndef LOCK_H_
#define LOCK_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define INCLUDE_vTaskSuspend  1 // Necessary for blocking the semaphores

#define WITH_SEMAPHORE(semaphore, code) \
    if (xSemaphoreTake(semaphore, portMAX_DELAY)) { \
        code \
    } \
    xSemaphoreGive(semaphore)

#endif // LOCK_H_
