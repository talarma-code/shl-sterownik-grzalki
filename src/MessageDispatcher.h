#pragma once
#include <Arduino.h>
#include "ShlProtocolPacket.h"
#include "HeaterDirect.h"
#include "DDS661PowerMeter.h"
#include "ShlProtocolTransport.h"
#include "IShlProtocolReceiver.h"

// Output pins 
#define LED_PIN 2 
#define RELAY_PIN 33

class MessageDispatcher  {
public:
    MessageDispatcher();
    void handleMessage(const ShlProtocolPacket &pkt, const uint8_t *srcMac);
    void setup(IShlProtocolReceiver *receiver);


private:
    HeaterDirect heater1;
    HeaterDirect heater2;
    DDS661PowerMeter dds661PowerMeter;
    ShlProtocolTransport transport;
    uint8_t messageCounter{0};

    void handleSetRelays(const ShlProtocolPacket &pkt, const uint8_t *srcMac);
    void sendReportAll(const uint8_t *dstMac, uint8_t messageCounterOverride);
    void sendReportPower(const uint8_t *dstMac, uint8_t messageCounterOverride);
    void sendReportVoltage(const uint8_t *dstMac, uint8_t messageCounterOverride);
    uint16_t readTotalPower();
    uint16_t readVoltage();

    bool getRelayStateForEndpoint(uint8_t ep);
    void setRelayStateForEndpoint(uint8_t ep, bool state);
    void toggleRelay(uint8_t ep);
};
