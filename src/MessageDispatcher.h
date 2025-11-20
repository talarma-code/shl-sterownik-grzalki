#pragma once
#include <Arduino.h>
#include "MatterLikePacket.h"
#include "HeaterDirect.h"
#include "DDS661PowerMeter.h"
#include "EspNowTransport.h"
#include "IMatterReceiver.h"

// Output pins 
#define LED_PIN 2 
#define RELAY_PIN 33

class MessageDispatcher : public IMatterReceiver {
public:
    MessageDispatcher();
    void handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) override;


private:
    HeaterDirect heater1;
    HeaterDirect heater2;
    DDS661PowerMeter dds661PowerMeter;
    EspNowTransport transport;

    void handleOnOff(const MatterLikePacket &pkt, const uint8_t *srcMac);
    void handleElectricalMeasurement(const MatterLikePacket &pkt, const uint8_t *srcMac);

    bool getRelayStateForEndpoint(uint8_t ep);
    void setRelayStateForEndpoint(uint8_t ep, bool state);
    void toggleRelay(uint8_t ep);
};
