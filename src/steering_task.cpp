#include "steering_task.hpp"
#include "steering.hpp"

void onSteeringTimer(TimerHandle_t timer)
{
    //when the timer is up for steering do whatever user preferred
    auto steeringController = Steering::getInstance();
    steeringController->updateState();
}

void startSteering()
{
    auto steeringTimer = xTimerCreate(
        "periodicSteeringTask",
        pdMS_TO_TICKS(100),
        pdTRUE,//restart automatically
        NULL,
        onSteeringTimer
    );
    
    xTimerStart(steeringTimer,0);
}

