#pragma once
#include "HeaterDirect.h"
#include "MessageDispatcher.h"
#include "ActiveQueue.h"
#include "MatterLikePacket.h"

class Application : public IMatterReceiver  {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino
    void handlePacket(const MatterPacketWithMac &pkt) override;

private:
    MessageDispatcher messageDispatcher;
    ActiveQueue<MatterPacketWithMac> packetQueue;
    
};