#ifndef STEERING_HPP
#define STEERING_HPP 

class Steering
{
    public: 
        static Steering* getInstance();
        void updateState();

    private:
        enum class SteeringState{
            STEER_LEFT,
            STEER_STRAIGHT,
            STEER_RIGHT,
        };
        static Steering* instance;
        static constexpr float RAIL_VOLTAGE = 3.3;//power going into the BTS7960
        static const int ADC_RESOLUTION = 4096;
        static constexpr float CURRENT_SCALE_BTS = 8500.0;
        static const int RIGHT_IS_INPUT_PIN = 14;//current indication
        static const int LEFT_IS_INPUT_PIN= 15;// current indication
        static const int STEER_LEFT_PIN = 16;
        static const int STEER_RIGHT_PIN = 17;
        static const int STEERING_INPUT_PIN = 4; 
        static const int STEER_LEFT_CHANNEL = 0;
        static const int STEER_RIGHT_CHANNEL = 1;
        static const int PWM_FREQUENCY = 1000;//hertz
        static const int PWM_RESOLUTION = 8;//bit
        //from RC
        static const int MAX_STEER_LEFT_PWM = 2010;//todo:check this 
        static const int STRAIGHT_PWM = 1500; 
        static const int MAX_STEER_RIGHT_PWM = 980;
        Steering();
        ~Steering();
        SteeringState currentState = SteeringState::STEER_STRAIGHT;
        unsigned long pulseStart = 0;
        unsigned long pulseWidth = (unsigned long) STRAIGHT_PWM;
        static void handleSteeringInterrupt();
        void steerLeft(float percentMax);
        void steerRight(float percentMax);
        void steerStraight();
        void setPulseWidth(const unsigned long pulseWidth);
        void handleSteeringInterruptImpl();
        float getCurrent(int pinNum);
        // Delete copy constructor and assignment operator
        Steering(const Steering&) = delete;
        Steering& operator=(const Steering&) = delete;
};


#endif