#include "Application.h"
#include "Log.h"
#include <Arduino.h>



Application::Application() : messageDispatcher(), packetQueue(3),
timer(APPLICATION_SYSTEM_TIMER_ID, 5000, SystemTimerT<SystemMessage, TimerToSystemMessage>::Mode::OneShot, ActiveQueueRef<SystemMessage>(packetQueue.nativeHandle()), TimerToSystemMessage()),
buttonHandler(4, ActiveQueueRef<SystemMessage>(packetQueue.nativeHandle())) // pin przycisku
{}

void Application::setup() {

    Serial.begin(115200);
    Logger::instance().begin(&Serial, Logger::Level::Debug);
    Logger::instance().log(Logger::Level::Info, "APP", "Logger initialized");
    delay(500);
    buttonHandler.begin();
    messageDispatcher.setup(this);

    pinMode(19, OUTPUT);
    pinMode(21, OUTPUT);
    digitalWrite(19, LOW);
    digitalWrite(21, LOW);

    LOG_INFO("Application setup complete - v1.01");
}

void Application::loop() {

    // Process button state changes
    buttonHandler.update();

    //ShlProtocolWithMacAddress rx;
    SystemMessage msg;
    // Use short timeout to allow button update() to run frequently
    if (packetQueue.receive(msg, 50)) {

        if (state == STATE_AUTOMATION) {
            if (msg.type == SystemMessageType::ButtonEvent) {
                switch(buttonHandler.lastPressCategory()) {
                    case ButtonHandler::PressCategory::Short:
                        LOG_INFO("Enable heater for 2H");
                        state = STATE_MANULA_ENABLED;
                        messageDispatcher.setHeater1Override(true);
                        messageDispatcher.setHeater2Override(true);
                        digitalWrite(19, HIGH);
                        //TODO: check it for long time, do not work correclty
                        timer.start(12UL * 60UL * 60UL * 1000UL); // this is potential error for value 3 timer was enable for about 38 minutes
                        break;

                    case ButtonHandler::PressCategory::Long:
                        state = STATE_MANULA_ENABLED;
                        LOG_INFO("Enable heater - required manual disable");
                        messageDispatcher.setHeater1Override(true);
                        messageDispatcher.setHeater2Override(true);
                        digitalWrite(19, HIGH);
                        digitalWrite(21, HIGH);
                        break;
                }
            }
            
        }
        else if (state == STATE_MANULA_ENABLED) {
            if (msg.type == SystemMessageType::ButtonEvent) {
                LOG_INFO("Manual override - disabling heaters");
                messageDispatcher.setHeater1Override(false);
                messageDispatcher.setHeater2Override(false);
                digitalWrite(19, LOW);
                digitalWrite(21, LOW);
                state = STATE_AUTOMATION;
                timer.stop();
            }

            if (msg.type == SystemMessageType::TimerEvent) {
                LOG_INFO("Time end - disabling heaters");
                messageDispatcher.setHeater1Override(false);
                messageDispatcher.setHeater2Override(false);
                digitalWrite(19, LOW);
                digitalWrite(21, LOW);
                state = STATE_AUTOMATION;
                timer.stop();
            }
        }

        if (msg.type == SystemMessageType::NotifyEspNowEvent) {
            Serial.println("Application::loop - packet received from queue:");
            Serial.printf("CMD=%u, msgCounter=%u, r1=%u, r2=%u, totalPower=%u, voltage=%u\n",
                        msg.protocolPacket.packet.commandId,
                        msg.protocolPacket.packet.messageCounter,
                        msg.protocolPacket.packet.relay1,
                        msg.protocolPacket.packet.relay2,
                        msg.protocolPacket.packet.totalPower,
                        msg.protocolPacket.packet.voltage);
            messageDispatcher.handleMessage(msg.protocolPacket.packet, msg.protocolPacket.mac.bytes);
        }

;
    }
}

// this is called from ISR context when packet is received
void Application::handlePacket(const ShlProtocolWithMacAddress &pkt) {
    SystemMessage msg;
    msg.type = SystemMessageType::NotifyEspNowEvent;
    msg.protocolPacket = pkt;
    packetQueue.sendFromISR(msg);
}

