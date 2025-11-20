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
    // dds661PowerMeter.setup();
}

void Application::loop() {
    // delay(1000);
    // Serial.print("Voltage: ");
    // Serial.printf("%.1f", dds661PowerMeter.voltage(DSS661_SLAVE_ADDRESS));
    // Serial.println("");

    // delay(1000);
    // Serial.print("activePower: ");
    // Serial.printf("%.1f", dds661PowerMeter.activePower(DSS661_SLAVE_ADDRESS));
    // Serial.println("");

    // delay(1000);
    // Serial.print("electricCurrent: ");
    // Serial.printf("%.1f", dds661PowerMeter.electricCurrent(DSS661_SLAVE_ADDRESS));
    // Serial.println("");

    // delay(1000);
    // Serial.print("frequency: ");
    // Serial.printf("%.1f", dds661PowerMeter.frequency(DSS661_SLAVE_ADDRESS));
    // Serial.println("");

    // delay(1000);
    // Serial.print("totalActivePower: ");
    // Serial.printf("%.1f", dds661PowerMeter.totalActivePower(DSS661_SLAVE_ADDRESS));
    // Serial.println("");
}

void Application::handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) {
  Serial.println("Received MatterLike packet!");
  messageDispatcher.handlePacket(pkt);
}
