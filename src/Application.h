#pragma once
#include "Heater/HeaterDirect.h"
#include "NetworkTransport/EspNowTransport.h"
#include "NetworkTransport/IMatterReceiver.h"
#include "MessageDispatcher.h"
#include "PowerMeter/PulsePowerMeter.h"
#include "PowerMeter/DDS661PowerMeter.h"


class Application : public IMatterReceiver {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino
    void handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) override;

private:
    HeaterDirect heater;
    MessageDispatcher messageDispatcher;
    EspNowTransport transport;
    DDS661PowerMeter dds661PowerMeter;
};