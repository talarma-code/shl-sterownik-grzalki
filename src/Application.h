#pragma once
#include "HeaterDirect.h"
#include "MessageDispatcher.h"

class Application  {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino

private:
    MessageDispatcher messageDispatcher;
};