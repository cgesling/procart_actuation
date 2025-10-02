#ifndef THROTTLE_HPP
#define THROTTLE_HPP

#include "freertos/FreeRTOS.h"  
#include "freertos/timers.h"
#include "digital_pot.hpp"
//The throttle class will read in values from the pulseIn- (some pin) 
class Throttle {
    public: 
        static Throttle* getInstance();
        ~Throttle() = default; 
        unsigned long getPulseWidth();
        void updateState();
        void stop();
    
    private:
        static const int THROTTLE_IN = 2; // pin number 
        static const int MICRO_SWITCH = 21;
        static const int REQUIRED_SHIFT_TIME_ALLOWANCE_MS = 100;
        enum class ThrottleState{
            FORWARD,
            STOPPED,
            REVERSE,
            SHIFTING
        };

        static const int MAX_FORWARD_PWM = 2010;
        static const int STOP_PWM = 1495; //micro seconds
        static const int MAX_REVERSE_PWM = 978;
        TimerHandle_t safetyTimer;
        static Throttle* instance; 
        DigitalPotentiometer* pot;
        ThrottleState  currentState = ThrottleState::STOPPED;
        static void handleInterruptRising();
        static void safteyTimerCallback(TimerHandle_t xTimer);
        void handleInterruptRisingImpl();
        void setPulseWidth(const unsigned long &newPulseWidth);
        void forward(float percentageOfMaxSpeedFoward);
        void reverse(float percentageOfMaxSpeedReverse);
        Throttle();
        unsigned long pulseStart = 0;
        unsigned long pulseWidth = (unsigned long) STOP_PWM;
        bool newPulse = false;
        Throttle(const Throttle&) = delete;
        Throttle& operator=(const Throttle&) = delete;
};

#endif