#include "Application.h"
#include <Arduino.h>
#include "MatterLikeDebugger.h"


Application::Application() : messageDispatcher(), packetQueue(3)
{}

void Application::setup() {

    Serial.begin(115200);
    delay(500);
    messageDispatcher.setup(this);
}

void Application::loop() {
    MatterPacketWithMac rx;
    packetQueue.receive(rx, portMAX_DELAY);
    Serial.println("Application::loop - packet received from queue:");
    MatterLikeDebugger::print(rx);
    messageDispatcher.handleMessage(rx.packet, rx.mac.bytes);
}

// this is called from ISR context when packet is received
void Application::handlePacket(const MatterPacketWithMac &pkt) {
    packetQueue.sendFromISR(pkt);
}

