#include "MessageDispatcher.h"
#include "ShlProtocol.h"

#define DSS661_SLAVE_ADDRESS 1
static const uint8_t MAC_LOCAL_HEATER[]  = {0x74, 0x61, 0x6C, 0x61, 0x72, 0x31}; // talar1 - heater
static const uint8_t MAC_CENTRALKA[]   = {0x74, 0x61, 0x6C, 0x61, 0x72, 0x30}; // talar0 - centrala

MessageDispatcher::MessageDispatcher() : heater1(RELAY_PIN), heater2(LED_PIN)
{
}

void MessageDispatcher::setup(IShlProtocolReceiver *receiver)
{
    Serial.println("MessageDispatcher::setup");
    dds661PowerMeter.setup();
    delay(20);
    transport.onPacketReceived(receiver);
    transport.begin(MAC_LOCAL_HEATER, MAC_CENTRALKA);
}

void MessageDispatcher::handleMessage(const ShlProtocolPacket &pkt, const uint8_t *srcMac)
{
    Serial.println("Received ShlProtocol packet!");

    switch (pkt.commandId)
    {
    case SHL_PROTOCOL_CMD_SET_RELAYS:
        handleSetRelays(pkt, srcMac);
        break;
    case SHL_PROTOCOL_CMD_REPORT_ALL:
        Serial.println("Report-all request received");
        sendReportAll(srcMac, pkt.messageCounter);
        break;
    case SHL_PROTOCOL_CMD_REPORT_POWER:
        Serial.println("Report-power request received");
        sendReportPower(srcMac, pkt.messageCounter); //TODO: shl-protocol, check it
        break;
    case SHL_PROTOCOL_CMD_REPORT_VOLTAGE:
        Serial.println("Report-voltage request received");
        sendReportVoltage(srcMac, pkt.messageCounter); //TODO: shl-protocol, check it
        break;
    default:
        Serial.printf("Unknown ShlProtocol commandId: 0x%02X\n", pkt.commandId);
        break;
    }
}

// ---------------- ShlProtocol Handling ----------------

void MessageDispatcher::handleSetRelays(const ShlProtocolPacket &pkt, const uint8_t *srcMac)
{
    setRelayStateForEndpoint(1, pkt.relay1 != 0);
    setRelayStateForEndpoint(2, pkt.relay2 != 0);

    sendReportAll(srcMac, pkt.messageCounter);
}

// Relay helpers:
bool MessageDispatcher::getRelayStateForEndpoint(uint8_t ep)
{
    switch (ep)
    {
    case 1:
        return heater1.isOn();
    case 2:
        return heater2.isOn();
    }
    return false;
}

void MessageDispatcher::setRelayStateForEndpoint(uint8_t ep, bool state)
{
    switch (ep)
    {
    case 1:
        state ? heater1.turnOn() : heater1.turnOff();
        break;
    case 2:
        state ? heater2.turnOn() : heater2.turnOff();
        break;
    default:
        Serial.printf("Invalid endpoint for On/Off: %d\n", ep);
        break;
    }
}

void MessageDispatcher::toggleRelay(uint8_t ep)
{
    bool current = getRelayStateForEndpoint(ep);
    setRelayStateForEndpoint(ep, !current);
}

void MessageDispatcher::sendReportAll(const uint8_t *dstMac, uint8_t messageCounterOverride)
{
    const uint16_t totalPower = readTotalPower();
    const uint16_t voltage = readVoltage();

    ShlProtocolPacket rs = ShlProtocol::createReportAll(
        messageCounterOverride,
        totalPower,
        voltage,
        getRelayStateForEndpoint(1) ? 1 : 0,
        getRelayStateForEndpoint(2) ? 1 : 0);

    transport.send(dstMac, rs);
}

void MessageDispatcher::sendReportPower(const uint8_t *dstMac, uint8_t messageCounterOverride)
{
    const uint16_t totalPower = readTotalPower();

    ShlProtocolPacket rs = ShlProtocol::createReportPower(
        messageCounterOverride,
        totalPower,
        getRelayStateForEndpoint(1) ? 1 : 0,
        getRelayStateForEndpoint(2) ? 1 : 0);

    transport.send(dstMac, rs);
}

void MessageDispatcher::sendReportVoltage(const uint8_t *dstMac, uint8_t messageCounterOverride)
{
    const uint16_t voltage = readVoltage();

    ShlProtocolPacket rs = ShlProtocol::createReportVoltage(
        messageCounterOverride,
        voltage,
        getRelayStateForEndpoint(1) ? 1 : 0,
        getRelayStateForEndpoint(2) ? 1 : 0);

    transport.send(dstMac, rs);
}

uint16_t MessageDispatcher::readTotalPower()
{
    const uint32_t power = dds661PowerMeter.totalActivePower(DSS661_SLAVE_ADDRESS);
    return static_cast<uint16_t>(power);
}

uint16_t MessageDispatcher::readVoltage()
{
    const uint32_t voltage = dds661PowerMeter.voltage(DSS661_SLAVE_ADDRESS);
    //return voltage as uint16_t with one decimal place (e.g. 230.5V -> 2305)
    return static_cast<uint16_t>(voltage * 10);
}
