#include "MessageDispatcher.h"
#include "ShlProtocol.h"
#include "Log.h"

#define DSS661_SLAVE_ADDRESS 1
static const uint8_t MAC_LOCAL_HEATER[]  = {0x74, 0x61, 0x6C, 0x61, 0x72, 0x31}; // talar1 - heater
static const uint8_t MAC_CENTRALKA[]   = {0x74, 0x61, 0x6C, 0x61, 0x72, 0x30}; // talar0 - centrala

static const uint8_t HEATER_DISABLE = 0;
static const uint8_t HEATER_ENABLE = 1;
static const uint8_t HEATER_ENABLE_MANUAL = 2;

MessageDispatcher::MessageDispatcher() : heater1(RELAY_PIN), 
heater2(RELAY2_PIN), 
pulsePowerMeter()
{
     
}

void MessageDispatcher::setup(IShlProtocolReceiver *receiver)
{
    Serial.println("MessageDispatcher::setup");
    pulsePowerMeter.setup(1); // Initialize the power meter hardware
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
uint8_t MessageDispatcher::getRelayStateForEndpoint(uint8_t ep)
{
    switch (ep)
    {
    case 1:
        return getHeater1State();
    case 2:
        return getHeater2State();
    }
    return false;
}

uint8_t MessageDispatcher::getHeater1State()
{
    if (heater1OverrideEnabled) {
        return HEATER_ENABLE_MANUAL;
    }
    else if (heater1.isOn()) {
        return HEATER_ENABLE;
    }
    else {
        return HEATER_DISABLE;
    }
}

uint8_t MessageDispatcher::getHeater2State()
{
    if (heater2OverrideEnabled) {
        return HEATER_ENABLE_MANUAL;
    }
    else if (heater2.isOn()) {
        return HEATER_ENABLE;
    }
    else {
        return HEATER_DISABLE;
    }
}





void MessageDispatcher::setRelayStateForEndpoint(uint8_t ep, bool state)
{
    switch (ep)
    {
    case 1:
        state ? heater1.turnOn() : turnOffHeater1();
        break;
    case 2:
        state ? heater2.turnOn() : turnOffHeater2();
        break;
    default:
        Serial.printf("Invalid endpoint for On/Off: %d\n", ep);
        break;
    }
}


void MessageDispatcher::setHeater1Override(bool enable)
{
    heater1OverrideEnabled = enable;
    if (enable) {
        LOG_INFO("Heater 1 override enabled. Turning on heater 1.");
        heater1.turnOn();
    } else {
        LOG_INFO("Heater 1 override disabled. Turning off heater 1.");
        turnOffHeater1();
    }
}

void MessageDispatcher::setHeater2Override(bool enable)
{
    heater2OverrideEnabled = enable;
    if (enable) {
        LOG_INFO("Heater 2 override enabled. Turning on heater 2.");
        heater2.turnOn();
    } else {
        LOG_INFO("Heater 2 override disabled. Turning off heater 2.");
        turnOffHeater2();
    }
}



void MessageDispatcher::turnOffHeater1()
{
    if (!heater1OverrideEnabled) {
        heater1.turnOff();
    }
    else {
        LOG_INFO("Heater 1 override is enabled, not turning off.");
    }
    
}

void MessageDispatcher::turnOffHeater2()
{
    if (!heater2OverrideEnabled) {
        heater2.turnOff();
    }
    else {
        LOG_INFO("Heater 2 override is enabled, not turning off.");
    }
}

void MessageDispatcher::sendReportAll(const uint8_t *dstMac, uint8_t messageCounterOverride)
{
    const uint16_t totalPower = readTotalPower();
    const uint16_t voltage = readVoltage();

    ShlProtocolPacket rs = ShlProtocol::createReportAll(
        messageCounterOverride,
        totalPower,
        voltage,
        getRelayStateForEndpoint(1),
        getRelayStateForEndpoint(2));

    transport.send(dstMac, rs);
    LOG_INFO("Sent report-all to MAC %02X:%02X:%02X:%02X:%02X:%02X, totalPower=%u, voltage=%u, relay1=%u, relay2=%u", 
             dstMac[0], dstMac[1], dstMac[2], dstMac[3], dstMac[4], dstMac[5],
             totalPower, voltage, getRelayStateForEndpoint(1), getRelayStateForEndpoint(2));
}

void MessageDispatcher::sendReportPower(const uint8_t *dstMac, uint8_t messageCounterOverride)
{
    const uint16_t totalPower = readTotalPower();

    ShlProtocolPacket rs = ShlProtocol::createReportPower(
        messageCounterOverride,
        totalPower,
        getRelayStateForEndpoint(1),
        getRelayStateForEndpoint(2));

    transport.send(dstMac, rs);
}

void MessageDispatcher::sendReportVoltage(const uint8_t *dstMac, uint8_t messageCounterOverride)
{
    const uint16_t voltage = readVoltage();

    ShlProtocolPacket rs = ShlProtocol::createReportVoltage(
        messageCounterOverride,
        voltage,
        getRelayStateForEndpoint(1),
        getRelayStateForEndpoint(2));

    transport.send(dstMac, rs);
}

uint16_t MessageDispatcher::readTotalPower()
{
    pulsePowerMeter.totalEnergyKWh();
    return static_cast<uint16_t>(pulsePowerMeter.totalEnergyKWh() * 1000.0f); // Convert kWh to Wh
}

uint16_t MessageDispatcher::readVoltage()
{
    return 0;
}
