#pragma once
#include "Heater/HeaterDirect.h"
#include "NetworkTransport/EspNowTransport.h"
#include "MessageDispatcher.h"

class Application {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino

private:
    HeaterDirect heater;
    MessageDispatcher messageDispatcher;
    EspNowTransport transport;

    void onPacket(const MatterLikePacket &pkt, const uint8_t *mac);
};