#include "throttle.hpp"
#include "digital_pot.hpp"
#include <Arduino.h>
#include "esp_log.h"

static const char* TAG = "THROTTLE"; 
Throttle* Throttle::instance = nullptr;

Throttle::Throttle(){
    //initialize the digital pot - which will publish signals as throttle
    pot = DigitalPotentiometer::getInstance();
    //attach a interrupt on the pin being read from flight controller 
    printf("Attaching Interrupt\n");
    pinMode(THROTTLE_IN, INPUT_PULLUP);
    auto in = digitalRead(THROTTLE_IN);
    printf("initial value : %lu \n", in);
    //set the microswitch pin out 
    pinMode(MICRO_SWITCH, OUTPUT);
    digitalWrite(MICRO_SWITCH, LOW);//this means stopped
    attachInterrupt(digitalPinToInterrupt(THROTTLE_IN), handleInterruptRising, CHANGE);
    //saftey timer 
    safetyTimer= xTimerCreate(
        "ResetTimer",        // Timer name
        pdMS_TO_TICKS(1000), // 1 second timeout
        pdTRUE,             // Auto-reload (false = one-shot)
        NULL,                // Timer ID
        Throttle::safteyTimerCallback        // Callback function
    );
    //xTimerStart(safetyTimer, 0);
}

Throttle* Throttle::getInstance()
{
    if(instance == nullptr){
        instance = new Throttle();
    }
    return instance;
}

void IRAM_ATTR Throttle::handleInterruptRising() {
    if(instance != nullptr) {
        instance->handleInterruptRisingImpl();
    }
}

void IRAM_ATTR Throttle::handleInterruptRisingImpl() {
    if(digitalRead(THROTTLE_IN) == HIGH) {
       pulseStart = micros(); 
    }

    if(digitalRead(THROTTLE_IN) == LOW)
    {
        setPulseWidth(micros() - pulseStart);
    }
}

void Throttle::safteyTimerCallback(TimerHandle_t xTimer)
{
    //Timer is up 
    // if(instance != nullptr)
    // {
    //     instance->stop();
    // }

}

unsigned long Throttle::getPulseWidth()
{
return pulseWidth;
}

void Throttle::updateState()
{
    if(currentState == ThrottleState::SHIFTING)
    {
        //There's a two second pause with shifting into a different state- so ignore
        return;
    }
    
    auto speed = getPulseWidth();
    printf("Reverse/Forward : %lu\n", speed);
    //get the desired FNR from Speed and Direction 
    //1490 - 1500 == STOPPED
    //MAX_REVERSE_PWM -> 1490 = Reverse
    //1500-> MAX_FORWARD_PWM = FORWARD
    
    if(speed > (STOP_PWM -5) && speed < (STOP_PWM + 5))
    {
        if(currentState != ThrottleState::STOPPED){
            stop();
            printf("STOPPED\n");
            return;
        }
    }else if ( speed > STOP_PWM + 5 && speed < MAX_FORWARD_PWM)
    {   
        //the range for forward is from STOP_PWM+ 5 to MAX_FORWARD_PWM 
        auto range = MAX_FORWARD_PWM - (STOP_PWM + 5);
        auto desiredPercentSpeed = (speed - (STOP_PWM + 5))/ (float)range;
        forward(desiredPercentSpeed);
    }else if (speed < STOP_PWM - 5 && speed > MAX_REVERSE_PWM)
    {
        //range in reverse is STOP_PWM--5 to MAX_REVERSE_PWM 
        auto range = (STOP_PWM - 5) - (MAX_REVERSE_PWM);
        auto desiredPercentSpeed = 1.0f - ((speed - MAX_REVERSE_PWM)/(float)range);
        reverse(desiredPercentSpeed);
    }
    else 
    {
        //some out of range value and we just need to stop 
        stop();
        return;
    }
}

void Throttle::setPulseWidth(const unsigned long &newPulseWidth)
{   
    if(newPulseWidth > MAX_FORWARD_PWM || newPulseWidth < MAX_REVERSE_PWM)
    {
        printf("Invalid value was passed as pulseWidth");
        stop(); 
        return;
    }
   
    this->pulseWidth = newPulseWidth;
    xTimerReset(safetyTimer, 0);//reset the saftey timer
}


void Throttle::stop()
{
    currentState = ThrottleState::STOPPED;
    pot->setPosition(0);// Immediately stop
    digitalWrite(MICRO_SWITCH, LOW);
}

/**
 * Range is (0-1.0) with 0 pretty much stopped and 1.0 max
 */
void Throttle::forward(float percentOfMaxSpeed)
{
    //desired wiper position is from 0-127 
    auto wiperPosition = DigitalPotentiometer::MAX_STEPS * percentOfMaxSpeed;
    //if user is already going forward we dont have to shift 
    if(currentState == ThrottleState::FORWARD) 
    {
        pot->setPosition(wiperPosition);
        printf("Foward : %f %\n", percentOfMaxSpeed);
        return;
    }
    //if the user wasn't in forward - we need to set the pot to zero - then flip the microSwitchState 
    currentState = ThrottleState::SHIFTING;
    printf("SHIFTING\n");
    pot->setPosition(0);
    digitalWrite(MICRO_SWITCH, LOW);//this should allow the shift 
    delay(REQUIRED_SHIFT_TIME_ALLOWANCE_MS);//there's some sort of required wait time
    //we can now shift into foward 
    currentState = ThrottleState::FORWARD;
    digitalWrite(MICRO_SWITCH,HIGH);
    pot->setPosition(wiperPosition);
    printf("Foward : %f %\n", percentOfMaxSpeed);
}

/**
 * Range is (0-1.0) with 0 pretty much stopped and 1.0 max
 */
void Throttle::reverse(float percentOfMaxSpeed)
{
    auto wiperPosition = DigitalPotentiometer::MAX_STEPS * percentOfMaxSpeed;
    //Reverse means the smaller microSecond pulse - the faster in reverse (opposite of forward)- but this should be built into the percent
    if(currentState == ThrottleState::REVERSE)
    {
        pot->setPosition(wiperPosition);
        printf("Reverse : %f %\n", percentOfMaxSpeed);
        return;
    }
    //if we weren't reverse we need to shift into reverse 
    currentState = ThrottleState::SHIFTING;
    printf("SHIFTING\n");
    pot->setPosition(0);
    digitalWrite(MICRO_SWITCH, LOW);//allow the shift
    delay(REQUIRED_SHIFT_TIME_ALLOWANCE_MS);
    currentState = ThrottleState::REVERSE;
    digitalWrite(MICRO_SWITCH,HIGH);
    pot->setPosition(wiperPosition);
    printf("Reverse : %f %\n", percentOfMaxSpeed);
}

