#pragma once
#include <Arduino.h>
#include "MatterLikeProtocol/MatterLikePacket.h"
#include "Heater/HeaterDirect.h"

// Cluster IDs
#define CLUSTER_ONOFF                  0x0006
#define CLUSTER_ELECTRICAL_MEAS        0x0B04

// OnOff Commands
#define CMD_OFF                        0x00
#define CMD_ON                         0x01
#define CMD_TOGGLE                     0x02

// OnOff Attributes (reported state read-back)
#define ATTR_ONOFF_STATE               0x0000

// Electrical Measurement Attributes
#define ATTR_EM_ACTIVE_POWER           0x0505
#define ATTR_EM_RMS_VOLTAGE            0x0508
#define ATTR_EM_RMS_CURRENT            0x050B
#define ATTR_EM_POWER_FACTOR           0x050D

// Output pins 
#define LED_PIN 2 
#define RELAY_PIN 33

class MessageDispatcher {
public:
    MessageDispatcher();
    void handlePacket(const MatterLikePacket &pkt);

private:
    HeaterDirect heater1;
    HeaterDirect heater2;

    void handleOnOff(const MatterLikePacket &pkt);
    void handleElectricalMeasurement(const MatterLikePacket &pkt);

    bool getRelayStateForEndpoint(uint8_t ep);
    void setRelayStateForEndpoint(uint8_t ep, bool state);
    void toggleRelay(uint8_t ep);
};
