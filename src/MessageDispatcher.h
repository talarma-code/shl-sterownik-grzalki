#pragma once
#include <Arduino.h>
#include "MatterLikePacket.h"
#include "HeaterDirect.h"
#include "DDS661PowerMeter.h"

// Output pins 
#define LED_PIN 2 
#define RELAY_PIN 33

class MessageDispatcher {
public:
    MessageDispatcher();
    void handlePacket(const MatterLikePacket &pkt);

private:
    HeaterDirect heater1;
    HeaterDirect heater2;
    DDS661PowerMeter dds661PowerMeter;

    void handleOnOff(const MatterLikePacket &pkt);
    void handleElectricalMeasurement(const MatterLikePacket &pkt);

    bool getRelayStateForEndpoint(uint8_t ep);
    void setRelayStateForEndpoint(uint8_t ep, bool state);
    void toggleRelay(uint8_t ep);
};
