#include "throttle_task.hpp"
#include "throttle.hpp"


static const char* TAG = "THROTTLE_TASK";

void onThrottleTimer(TimerHandle_t xTimer)
{
    //Do a few things every 100ms 
    auto throttle = Throttle::getInstance();
    throttle->updateState();

}

void throttleTaskStart(){
    TimerHandle_t throttleTimer = xTimerCreate(
        "throttleTask",
        pdMS_TO_TICKS(100),//every 100ms 
        pdTRUE, //Reset on Completition
        NULL,
        onThrottleTimer
    );
    xTimerStart(throttleTimer, 0);
}