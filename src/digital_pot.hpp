#ifndef DIGITAL_POT_HPP
#define DIGITAL_POT_HPP

#include <SPI.h>

class DigitalPotentiometer {
    public:
        // Static method to get singleton instance
        static DigitalPotentiometer* getInstance();
        static const int MAX_STEPS = 127;
        // Public methods
        void setVoltage(float voltage);
        void setPosition(int position);
        float getMinVoltage() const { return MIN_VOLTAGE; }
        float getMaxVoltage() const { return MAX_VOLTAGE; }
        int getMaxSteps() const { return MAX_STEPS; }
        float positionToVoltage(int position) const;
        ~DigitalPotentiometer() = default;

    private:
        static DigitalPotentiometer* instance;
        static const int CS_PIN = 5;   // GPIO 5
        static const int MOSI = 23; 
        static const int MISO = 19;
        static const int SCLK = 18;
        /**
         * We will be writting values between 0-255 to the board. This will translate to 0.5v -> 4.4v
         * which are dictated on the baord
         */
        static constexpr float MIN_VOLTAGE = 0.5f;  // P0B voltage
        static constexpr float MAX_VOLTAGE = 4.4f;  // P0A voltage

        DigitalPotentiometer();

        // Delete copy constructor and assignment operator
        DigitalPotentiometer(const DigitalPotentiometer&) = delete;
        DigitalPotentiometer& operator=(const DigitalPotentiometer&) = delete;

        // Private helper methods
        void initializeSPI();
        void setWiperPosition(int position);
        int voltageToPosition(float voltage);
};

#endif
