#pragma once
#include <Arduino.h>
#include <ModbusMaster.h>
#include "IMatterLikeTransport.h"

class DDS661PowerMeter {

    public: 
    void setup();
    void registerSender(IMatterLikeTransport* sender);
    float voltage(uint8_t slaveId);             //Voltage units 
    float electricCurrent(uint8_t slaveId);     //amperage
    float activePower(uint8_t slaveId);         //Kilowatts
    float frequency(uint8_t slaveId);           //hertz
    float totalActivePower(uint8_t slaveId);    //Kilowatt/hours

    private:
    ModbusMaster node;
    IMatterLikeTransport* _sender;

    void modbusError(uint8_t result);
    float getFloatValue();
};
