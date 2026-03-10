#include "Application.h"
#include <Arduino.h>


Application::Application() : messageDispatcher(), packetQueue(3)
{}

void Application::setup() {

    Serial.begin(115200);
    delay(500);
    messageDispatcher.setup(this);
}

void Application::loop() {
    ShlProtocolWithMacAddress rx;
    packetQueue.receive(rx, portMAX_DELAY);
    Serial.println("Application::loop - packet received from queue:");
    Serial.printf("CMD=%u, msgCounter=%u, r1=%u, r2=%u, totalPower=%u, voltage=%u\n",
                  rx.packet.commandId,
                  rx.packet.messageCounter,
                  rx.packet.relay1,
                  rx.packet.relay2,
                  rx.packet.totalPower,
                  rx.packet.voltage);
    messageDispatcher.handleMessage(rx.packet, rx.mac.bytes);
}

// this is called from ISR context when packet is received
void Application::handlePacket(const ShlProtocolWithMacAddress &pkt) {
    packetQueue.sendFromISR(pkt);
}

