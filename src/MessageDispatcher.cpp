#include "MessageDispatcher.h"
#include "MatterLike.h"

#define DSS661_SLAVE_ADDRESS 1

MessageDispatcher::MessageDispatcher() : heater1(RELAY_PIN), heater2(LED_PIN)
{

}

void MessageDispatcher::setup() {
    Serial.println("MessageDispatcher::setup");
    dds661PowerMeter.setup();
    delay(20);
    transport.onPacketReceived(this);
    transport.begin();
}

void MessageDispatcher::handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac)
{
    Serial.println("Received MatterLike packet!");
    switch (pkt.payload.clusterId)
    {
    case CLUSTER_ONOFF:
        handleOnOff(pkt, srcMac);
        break;

    case CLUSTER_ELECTRICAL_MEAS:
        handleElectricalMeasurement(pkt, srcMac);
        break;

    default:
        Serial.printf("Unknown cluster ID: 0x%04X\n", pkt.payload.clusterId);
        break;
    }
}

// ---------------- OnOff Handling ----------------

void MessageDispatcher::handleOnOff(const MatterLikePacket &pkt, const uint8_t *srcMac)
{
    bool newState = false;

    switch (pkt.payload.commandId)
    {
    case CMD_ON:
        newState = true;
        Serial.println("CMD_ON received");
        setRelayStateForEndpoint(pkt.payload.endpointId, newState);
        break;

    case CMD_OFF:
        newState = false;
        Serial.println("CMD_OFF received");
        setRelayStateForEndpoint(pkt.payload.endpointId, newState);
        break;

    case CMD_TOGGLE:
        Serial.println("CMD_TOGGLE received");
        toggleRelay(pkt.payload.endpointId);
        break;

    case CMD_READ_ATTRIBUTE:
    {
        Serial.println("CMD_READ_ATTRIBUTE received");
        bool currentState = getRelayStateForEndpoint(pkt.payload.endpointId);
        Serial.printf("currentState: %d W\n", currentState); 
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt, currentState);
        transport.send(srcMac, rs);
        break;
        break;
    }

    default:
        Serial.printf("Unknown OnOff Command: 0x%02X\n", pkt.payload.commandId);
        return;
    }

    // NOTE: Matter would now send ReportAttribute (state change notification)
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

void MessageDispatcher::setRelayStateForEndpoint (uint8_t ep, bool state)
{
    switch (ep)
    {
    case 1:
        state ? heater1.turnOn() : heater1.turnOff();
        break;
    case 2:
        state ? heater2.turnOn() : heater1.turnOff();
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

// ---------------- Electrical Measurement Handling ----------------

void MessageDispatcher::handleElectricalMeasurement(const MatterLikePacket &pkt, const uint8_t *srcMac)
{
    // float val = (float)pkt.payload.value;

    switch (pkt.payload.attributeId)
    {
    case ATTR_EM_ACTIVE_POWER:
    {
        uint32_t power = dds661PowerMeter.activePower(DSS661_SLAVE_ADDRESS);
        Serial.printf("Active Power: %.2f W\n", power);
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt, power);
        transport.send(srcMac, rs);
        break;
    }

    case ATTR_EM_RMS_VOLTAGE:
    {
        uint32_t voltage = dds661PowerMeter.voltage(DSS661_SLAVE_ADDRESS);
        Serial.printf("Voltage: %.2f V\n", voltage);
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt, voltage);
        transport.send(srcMac, rs);
        break;
    }
    case ATTR_EM_RMS_CURRENT:
    {
        uint32_t current = dds661PowerMeter.electricCurrent(DSS661_SLAVE_ADDRESS);
        Serial.printf("Current: %.2f A\n", current);
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt, current);
        transport.send(srcMac, rs);
        break;
    }
    case ATTR_EM_ENERGY:
    {
        u32_t energy = dds661PowerMeter.totalActivePower(DSS661_SLAVE_ADDRESS);
        Serial.printf("Total active power/energy: %.2f\n", energy);
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt, energy);
        transport.send(srcMac, rs);
        break;
    }
    case ATTR_EM_RMS_FREQUENCY:
    {
        u32_t frequency = dds661PowerMeter.frequency(DSS661_SLAVE_ADDRESS);
        Serial.printf("Frequency: %.2f\n", frequency);
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt, frequency);
        transport.send(srcMac, rs);
        break;
    }
    default:
        Serial.printf("Unknown EM attribute: 0x%04X",
                      pkt.payload.attributeId);
        break;
    }
}
