#include "steering.hpp"
#include "Arduino.h"

 Steering* Steering::instance = nullptr;

Steering::Steering()
{
    //Setup 2 channels for steer left and steer right 
    ledcSetup(STEER_LEFT_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(STEER_LEFT_PIN, STEER_LEFT_CHANNEL);
    ledcSetup(STEER_RIGHT_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(STEER_RIGHT_PIN, STEER_RIGHT_CHANNEL);
    pinMode(STEERING_INPUT_PIN, INPUT); // input from rc-controller
    analogSetWidth(12); //set Adc to a 12-bit resolution (0-4095) -> ADC_RESOLUTION
    analogSetPinAttenuation(RIGHT_IS_INPUT_PIN, ADC_11db);//setting this pin to 3.3v Attenutation range 
    analogSetPinAttenuation(LEFT_IS_INPUT_PIN, ADC_11db);
    pinMode(RIGHT_IS_INPUT_PIN, INPUT);//current sensing ADC
    pinMode(LEFT_IS_INPUT_PIN, INPUT);//current sensing adc
    //attach an interupt to the input pin 
    attachInterrupt(digitalPinToInterrupt(STEERING_INPUT_PIN), handleSteeringInterrupt, CHANGE);
}

void IRAM_ATTR Steering::handleSteeringInterrupt()
{
    if(instance != nullptr)
    {
        instance-> handleSteeringInterruptImpl();
    }
}

void Steering::steerRight(float percentMax)
{
    uint8_t steerValue = 255 * percentMax; // this is just taking percent and getting it to the normalized resolution of the channel
    if(currentState == SteeringState::STEER_RIGHT)
    {
        //just write the higher value to the steer channel 
        ledcWrite(STEER_RIGHT_CHANNEL, steerValue);
        getCurrent(LEFT_IS_INPUT_PIN);//just checking current here
        getCurrent(RIGHT_IS_INPUT_PIN);//just checking current currently
        return;
    }
    //Wasn't steering right so start steering right and set the value 
    ledcWrite(STEER_LEFT_CHANNEL, 0);
    ledcWrite(STEER_RIGHT_CHANNEL, steerValue);
    currentState = SteeringState::STEER_RIGHT;
}

void Steering::steerLeft(float percentMax)
{
    uint8_t steerValue = 255 * percentMax; 

    if(currentState == SteeringState::STEER_LEFT)
    {
        //just write higher 
        ledcWrite(STEER_LEFT_CHANNEL, steerValue);
        getCurrent(LEFT_IS_INPUT_PIN); //print adc value of current
        getCurrent(RIGHT_IS_INPUT_PIN); //print adc value of current
        return;
    }
    ledcWrite(STEER_RIGHT_CHANNEL, 0);
    ledcWrite(STEER_LEFT_CHANNEL, steerValue);
    currentState = SteeringState::STEER_LEFT;
}

void Steering::steerStraight()
{
    ledcWrite(STEER_LEFT_CHANNEL,0);
    ledcWrite(STEER_RIGHT_CHANNEL, 0);
    currentState = SteeringState::STEER_STRAIGHT;
}


void Steering::updateState()
{
    //There's nothing to shift yet (when steering back in forth except ot turn off the other pins 

    //TODO-- this is the place to put the check for the Throttle - if no throttle you may not want to steer
    /*
    bool isMoving = Throttle::getInstance()->getState() != ThrottleState::Stopped;
    if(!isMoving){
    return; //use this return early
    }
    */
    auto newPulseWidth = pulseWidth; 
    printf("PulseWidth-Left/Right: %lu\n", newPulseWidth);
    //Make sure the new Pulse Width is in a valid range- if it's not- just turn off controls
    if(newPulseWidth > MAX_STEER_LEFT_PWM && newPulseWidth < MAX_STEER_RIGHT_PWM){
        steerStraight();
        return;
    }

    if(newPulseWidth > (STRAIGHT_PWM - 5) && newPulseWidth < (STRAIGHT_PWM + 5))
    {
        if(currentState != SteeringState::STEER_STRAIGHT)
        {
            steerStraight();
        }
    }else if ( newPulseWidth > STRAIGHT_PWM + 5 && newPulseWidth < MAX_STEER_LEFT_PWM)
    {
        auto range = MAX_STEER_LEFT_PWM - (STRAIGHT_PWM + 5);
        auto desiredPercentSteerLeft = (newPulseWidth - ( STRAIGHT_PWM + 5))/ (float) range;
        steerLeft(desiredPercentSteerLeft);
    }else if ( newPulseWidth < STRAIGHT_PWM - 5 && newPulseWidth >= MAX_STEER_RIGHT_PWM)
    {
        auto range = (STRAIGHT_PWM - 5) - (MAX_STEER_RIGHT_PWM);
        auto desiredPercentSpeed = 1.0f - ((newPulseWidth - MAX_STEER_RIGHT_PWM)/(float)range);
        steerRight(desiredPercentSpeed);
    }else
    {
        //really bad value here for some reason
        steerStraight();
        return;
    }
}

void IRAM_ATTR Steering::handleSteeringInterruptImpl()
{
    if(digitalRead(STEERING_INPUT_PIN) == HIGH)
    {
        pulseStart = micros();
        return;
    }
    if(digitalRead(STEERING_INPUT_PIN) == LOW)
    {
        setPulseWidth(micros() - pulseStart);
    }
}

float Steering::getCurrent(int pin){
    //int adcValue = analogRead(pin);
    //printf("Current on (Pin: %d) - %d\n", pin,adcValue);
    // float voltage = (adcValue * RAIL_VOLTAGE) / (float)ADC_RESOLUTION;

    // float current = voltage / (1.5 /());
}

void Steering::setPulseWidth(const unsigned long newPulseWidth)
{   
    pulseWidth = newPulseWidth;
}

Steering* Steering::getInstance()
{
    if(instance == nullptr)
    {
        instance = new Steering();
    }
    return instance;
}