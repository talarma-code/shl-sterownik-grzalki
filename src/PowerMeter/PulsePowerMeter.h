#pragma once
#include <Arduino.h>

/*
    To use this counter with standard industry SO power meter it is require higer voltage than 3.3V. Do not work for 3.3V, 
    not tested for higer voltage. Counter configuration working (is able to count). 
    Required testing and if needed add some filterni to avoid noise. 
*/
class PulsePowerMeter {
    public:
    PulsePowerMeter(uint32_t pin);

    void setup();
    uint32_t count();

    private:
    uint32_t _pin;
};