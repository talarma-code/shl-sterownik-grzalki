#include "Application.h"
#include <Arduino.h>


Application::Application()
{}

void Application::setup() {

    Serial.begin(115200);
    delay(500);
    messageDispatcher.setup();
}

void Application::loop() {
     delay(1000);
}

