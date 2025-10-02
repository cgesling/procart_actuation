#include <Arduino.h>
#include "digital_pot.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "throttle_task.hpp"
#include "throttle.hpp"
#include "steering.hpp"
#include "steering_task.hpp"

// put function declarations here
Throttle* throttle = nullptr;
Steering* steer = nullptr;
void setup() {
    // xTaskCreate(
    //     throttleTaskStart,          // Task function
    //     "PotentiometerTask",        // Task name (for debugging)
    //     4096,                       // Stack size (bytes)
    //     NULL,                       // Task parameter
    //     2,                          // Task priority (0-25, higher = more priority)
    //     NULL   // Task handle (optional)
    // );
    throttle = Throttle::getInstance();
    steer = Steering::getInstance();
    throttleTaskStart();
    startSteering();
}



void loop() {
  //Dont use run a rtos task

}


/*
Digital Pot controls the Speed (Throttle)
Enable controls the throttle micro-switch 

FNR - Forward Neutral Reverse 

Transmission controller 
  - some amount is foward 
  - some amount is reverse 
*/


