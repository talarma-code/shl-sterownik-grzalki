#pragma once
#include "Heater/HeaterDirect.h"

class Application {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino

private:
    HeaterDirect heater;
};