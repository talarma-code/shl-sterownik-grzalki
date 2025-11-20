#include "MessageDispatcher.h"
#include "MatterLike.h"

#define DSS661_SLAVE_ADDRESS 1

MessageDispatcher::MessageDispatcher() : heater1(RELAY_PIN), heater2(LED_PIN)
{
    dds661PowerMeter.setup();
}

void MessageDispatcher::handlePacket(const MatterLikePacket &pkt)
{
    switch (pkt.payload.clusterId)
    {
    case CLUSTER_ONOFF:
        handleOnOff(pkt);
        break;

    case CLUSTER_ELECTRICAL_MEAS:
        handleElectricalMeasurement(pkt);
        break;

    default:
        Serial.printf("Unknown cluster ID: 0x%04X\n", pkt.payload.clusterId);
        break;
    }
}

// ---------------- OnOff Handling ----------------

void MessageDispatcher::handleOnOff(const MatterLikePacket &pkt)
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
        return true; // add implementation
    case 2:
        return true; // add implementation
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

void MessageDispatcher::handleElectricalMeasurement(const MatterLikePacket &pkt)
{
    float val = (float)pkt.payload.value;

    switch (pkt.payload.attributeId)
    {
    case ATTR_EM_ACTIVE_POWER:
    {
        uint32_t power = dds661PowerMeter.activePower(DSS661_SLAVE_ADDRESS);
        Serial.printf("Active Power: %.2f W\n", power);
        MatterLikePacket rs = MatterLike::createReportAttributePacket(pkt.payload.nodeId,
                                                                      pkt.payload.endpointId,
                                                                      pkt.payload.clusterId,
                                                                      pkt.payload.attributeId,
                                                                      power);

        break;
    }

    case ATTR_EM_RMS_VOLTAGE:
        Serial.printf("Voltage: %.2f V\n", val);
        break;
    case ATTR_EM_RMS_CURRENT:
        Serial.printf("Current: %.2f A\n", val);
        break;
    case ATTR_EM_POWER_FACTOR:
        Serial.printf("Power Factor: %.2f\n", val);
        break;
    default:
        Serial.printf("Unknown EM attribute: 0x%04X value=%f\n",
                      pkt.payload.attributeId, val);
        break;
    }
}
