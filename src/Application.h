#pragma once
#include "HeaterDirect.h"
#include "MessageDispatcher.h"
#include "ActiveQueue.h"
#include "ShlProtocolPacket.h"
#include "IShlProtocolReceiver.h"

class Application : public IShlProtocolReceiver  {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino
    void handlePacket(const ShlProtocolWithMacAddress &pkt) override;

private:
    MessageDispatcher messageDispatcher;
    ActiveQueue<ShlProtocolWithMacAddress> packetQueue;
    
};