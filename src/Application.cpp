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
    transport.onPacketReceived([](void *ctx, const MatterLikePacket &pkt, const uint8_t *mac) {
        static_cast<Application*>(ctx)->onPacket(pkt, mac);
    },
    this
);
}

void Application::loop() {
    Serial.println("szybko mrugacz");
    heater.turnOn();
    delay(3000);
    heater.turnOff();
    delay(3000);
}


void Application::onPacket(const MatterLikePacket &pkt, const uint8_t *mac) {
  Serial.println("Received MatterLike packet!");
  messageDispatcher.handlePacket(pkt);
}