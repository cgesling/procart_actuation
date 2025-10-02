#ifndef STEERING_TASK_HPP
#define STEERING_TASK_HPP
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

void startSteering();
void onSteeringTimer(TimerHandle_t xTimer);
#endif 