#pragma once
#include "HeaterDirect.h"
#include "MessageDispatcher.h"
#include "ActiveQueue.h"
#include "ShlProtocolPacket.h"
#include "IShlProtocolReceiver.h"
#include "SystemTimer.h"
#include "ButtonHandler.h"


class TimerToSystemMessage {
    public:
    SystemMessage operator()(const TimerEvent& t) const noexcept {
        SystemMessage m{};
        m.type = SystemMessageType::TimerEvent;
        return m;
    }
};

class Application : public IShlProtocolReceiver  {
public:
    Application();        // konstruktor
    void setup();         // setup Arduino
    void loop();          // loop Arduino
    void handlePacket(const ShlProtocolWithMacAddress &pkt) override;

private:
    MessageDispatcher messageDispatcher;
    ActiveQueue<SystemMessage> packetQueue;
    SystemTimerT<SystemMessage, TimerToSystemMessage> timer;
    ButtonHandler buttonHandler;

    static const uint32_t APPLICATION_SYSTEM_TIMER_ID = 1;

    enum ApplicationStateEnum {
        STATE_AUTOMATION,
        STATE_MANULA_ENABLED
    };
 
    ApplicationStateEnum state = STATE_AUTOMATION;

    
};