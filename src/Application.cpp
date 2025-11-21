#include "Application.h"
#include <Arduino.h>
#include <WiFi.h>


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

