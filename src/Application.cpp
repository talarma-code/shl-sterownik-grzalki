#include "Application.h"
#include <Arduino.h>

#define LED_PIN 2  // connected relay 

Application::Application()
    : heater(LED_PIN)       // przypisanie pinu grzałki
{}

void Application::setup() {
    Serial.begin(115200);
    heater.turnOff();
    transport.begin();
    transport.onPacketReceived(this);

}

void Application::loop() {
    Serial.println("czekam na dane...");
    delay(3000);
}

void Application::handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) {
  Serial.println("Received MatterLike packet!");
  messageDispatcher.handlePacket(pkt);
}
