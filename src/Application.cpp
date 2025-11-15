#include "Application.h"
#include <Arduino.h>
#include <WiFi.h>

#define LED_PIN 2  // connected relay 

Application::Application()
    : heater(LED_PIN)       // przypisanie pinu grzałki
{}

void Application::setup() {

    Serial.begin(115200);
    delay(500);
    heater.turnOff();
    transport.begin();
    transport.onPacketReceived(this);

}

void Application::loop() {
    Serial.println("czekam na dane...");
     Serial.println(WiFi.macAddress());
    delay(3000);
}

void Application::handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) {
  Serial.println("Received MatterLike packet!");
  messageDispatcher.handlePacket(pkt);
}
