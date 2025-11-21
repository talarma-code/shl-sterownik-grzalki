#pragma once

#include <Arduino.h>


// Flags for MatterLikeHeader (1 byte)
#define ML_FLAG_ACK_REQUEST   0x01  // Bit 0: Packet requires acknowledgement (ACK request)
#define ML_FLAG_ACK_RESPONSE  0x02  // Bit 1: Packet is a transport acknowledgement (ACK response)
#define ML_FLAG_RETRY         0x04  // Bit 2: Retransmission / retry packet
#define ML_FLAG_BROADCAST     0x08  // Bit 3: Broadcast packet
#define ML_FLAG_ENCRYPTED     0x10  // Bit 4: Payload is encrypted
#define ML_FLAG_RESERVED1     0x20  // Bit 5: Reserved for future use
#define ML_FLAG_RESERVED2     0x40  // Bit 6: Reserved for future use
#define ML_FLAG_RESERVED3     0x80  // Bit 7: Reserved for future use


// Cluster IDs
#define CLUSTER_ONOFF                  0x0006
#define CLUSTER_ELECTRICAL_MEAS        0x0B04

// OnOff Commands
#define CMD_OFF                        0x00
#define CMD_ON                         0x01
#define CMD_TOGGLE                     0x02
#define CMD_READ_ATTRIBUTE             0x03
#define CMD_REPORT_ATTRIBUTE           0x04      

// OnOff Attributes (reported state read-back)
#define ATTR_ONOFF_STATE               0x0000

// Electrical Measurement Attributes
#define ATTR_EM_ACTIVE_POWER            0x0505   // Active Power (W)
#define ATTR_EM_RMS_VOLTAGE             0x0508   // RMS Voltage (V)
#define ATTR_EM_RMS_CURRENT             0x050B   // RMS Current (mA)
#define ATTR_EM_POWER_FACTOR            0x050D   // Power Factor (0–100)

#define ATTR_EM_APPARENT_POWER          0x0510   // Apparent Power (VA)
#define ATTR_EM_REACTIVE_POWER          0x0515   // Reactive Power (VAR)
#define ATTR_EM_ENERGY                  0x051A   // Energy Consumed (Wh)
#define ATTR_EM_RMS_FREQUENCY           0x051C   // Frequency (Hz)
#define ATTR_EM_CURRENT_SUMMATION       0x0520   // Total Current Summation (mAh)

struct MatterLikeHeader {
  uint32_t messageCounter;
  uint16_t sessionId;
  uint8_t flags;
};

struct MatterLikePayload {
  uint32_t fabricId;        // ID sieci / instalacji
  uint16_t nodeId;          // ID urządzenia (gniazdko)
  uint8_t endpointId;       // ID funkcji / endpointu
  uint16_t clusterId;       // typ klastra (OnOff lub ElectricalMeasurement)
  uint16_t attributeId;     // ID atrybutu w klastrze
  uint8_t commandId;        // np. 0x03 = ReadAttribute, 0x04 = ReportAttribute
  int32_t value;            // dla danych liczbowych lub bool (0/1)
};

struct MatterLikePacket {
  MatterLikeHeader header;
  MatterLikePayload payload;
};