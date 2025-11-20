#pragma once
#include "HeaterDirect.h"
// #include "EspNowTransport.h"
// #include "IMatterReceiver.h"
#include "MessageDispatcher.h"
// #include "PulsePowerMeter.h"



class Application  {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino

private:
    HeaterDirect heater;
    MessageDispatcher messageDispatcher;
};