#pragma once
#include <Arduino.h>
#include "ShlProtocolPacket.h"
#include "HeaterDirect.h"
#include "DDS661PowerMeter.h"
#include "ShlProtocolTransport.h"
#include "IShlProtocolReceiver.h"
#include "PulsePowerMeter.h"

// Output pins 
#define RELAY_PIN 33
#define RELAY2_PIN 2

class MessageDispatcher  {
public:
    MessageDispatcher();
    void handleMessage(const ShlProtocolPacket &pkt, const uint8_t *srcMac);
    void setup(IShlProtocolReceiver *receiver);
    void setHeater1Override(bool enable);
    void setHeater2Override(bool enable);


private:
    HeaterDirect heater1;
    HeaterDirect heater2;
    bool heater1OverrideEnabled = false;
    bool heater2OverrideEnabled = false;
    PulsePowerMeter pulsePowerMeter;
    ShlProtocolTransport transport;
    uint8_t messageCounter{0};

    void handleSetRelays(const ShlProtocolPacket &pkt, const uint8_t *srcMac);
    void sendReportAll(const uint8_t *dstMac, uint8_t messageCounterOverride);
    void sendReportPower(const uint8_t *dstMac, uint8_t messageCounterOverride);
    void sendReportVoltage(const uint8_t *dstMac, uint8_t messageCounterOverride);
    uint16_t readTotalPower();
    uint16_t readVoltage();

    uint8_t getHeater1State();
    uint8_t getHeater2State();

    uint8_t getRelayStateForEndpoint(uint8_t ep);
    void setRelayStateForEndpoint(uint8_t ep, bool state);

    void turnOffHeater1();
    void turnOffHeater2();
};
