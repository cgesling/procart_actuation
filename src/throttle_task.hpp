#ifndef THROTTLE_TASK_HPP
#define THROTTLE_TASK_HPP 
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

void throttleTaskStart();
void onThrottleTimer(TimerHandle_t xTimer);
#endif