#include "Application.h"
#include <Arduino.h>
#include <WiFi.h>

#define LED_PIN 2  // connected relay 

Application::Application()
    : heater(LED_PIN),       // przypisanie pinu grzałki
    pulsePowerMeter(4)      //GPIO 4 as pulse inpute
{}

void Application::setup() {

    Serial.begin(115200);
    delay(500);
    heater.turnOff();
    transport.begin();
    transport.onPacketReceived(this);
    //pulsePowerMeter.setup();              //uncomment to configure pulse counter for power meter           

}

void Application::loop() {
    delay(2000);
    //uint32_t count = pulsePowerMeter.count();
}

void Application::handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) {
  Serial.println("Received MatterLike packet!");
  messageDispatcher.handlePacket(pkt);
}
