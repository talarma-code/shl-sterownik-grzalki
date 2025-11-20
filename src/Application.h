#pragma once
#include "Heater/HeaterDirect.h"
#include "EspNowTransport.h"
#include "IMatterReceiver.h"
#include "MessageDispatcher.h"
#include "PulsePowerMeter.h"



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
};