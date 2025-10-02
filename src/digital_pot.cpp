#include "digital_pot.hpp"
#include <Arduino.h>

DigitalPotentiometer* DigitalPotentiometer::instance = nullptr;

DigitalPotentiometer::DigitalPotentiometer() {
    initializeSPI();
}


DigitalPotentiometer* DigitalPotentiometer::getInstance() {
    if (instance == nullptr) {
        instance = new DigitalPotentiometer();
    }
    return instance;
}


void DigitalPotentiometer::initializeSPI() {
    // Initialize SPISPI.begin(SCK_PIN, MISO_PIN, SDI_SDO_PIN, CS_PIN);
    SPI.begin(SCK, MISO, MOSI, CS_PIN);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV16);  // Conservative clock speed
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);  // CS idle high
}

void DigitalPotentiometer::setVoltage(float voltage) {
    if (voltage < MIN_VOLTAGE || voltage > MAX_VOLTAGE) {
        
        return;
    }
    
    int position = voltageToPosition(voltage);
    setWiperPosition(position);
    
    Serial.print("Set voltage: ");
    Serial.print(voltage, 3);
    Serial.print("V (position ");
    Serial.print(position);
    Serial.println(")");
}

// Set position directly (0-128) 7-bit
void DigitalPotentiometer::setPosition(int position) {
    if (position < 0){
        position = 0;
    }else if(position > MAX_STEPS){
        position = MAX_STEPS;
    }
    
    
    setWiperPosition(position);
    float voltage = positionToVoltage(position);
}

// Convert voltage to position
int DigitalPotentiometer::voltageToPosition(float voltage) {
    float voltageRange = MAX_VOLTAGE - MIN_VOLTAGE;
    float voltageOffset = voltage - MIN_VOLTAGE;
    
    int position = (int)((voltageOffset / voltageRange) * MAX_STEPS);
    return constrain(position, 0, MAX_STEPS);
}

// Convert position to voltage
float DigitalPotentiometer::positionToVoltage(int position) const {
    float voltageRange = MAX_VOLTAGE - MIN_VOLTAGE;
    auto volts = MIN_VOLTAGE+ ((float)position / MAX_STEPS) * voltageRange;

    printf("Pot set to : %.2f volts\n", volts);
    return MIN_VOLTAGE + ((float)position / MAX_STEPS) * voltageRange;
}

// Send SPI command to set wiper position
void DigitalPotentiometer::setWiperPosition(int position) {
    digitalWrite(CS_PIN, LOW);   // Select chip
    delayMicroseconds(1);        // Small delay
    
    // Send 8-bit position value
    SPI.transfer(0x00);//command byte
    SPI.transfer(position);
    
    delayMicroseconds(1);        // Small delay
    digitalWrite(CS_PIN, HIGH);  // Deselect chip
}

