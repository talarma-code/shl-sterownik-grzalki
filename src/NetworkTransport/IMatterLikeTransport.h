#pragma once
#include <Arduino.h>
#include "IMatterReceiver.h"
#include "MatterLikePacket.h"


class IMatterLikeTransport {
public:
  virtual ~IMatterLikeTransport() = default;

  // Initialize transport (radio, sockets, etc.)
  virtual bool begin() = 0;

  // Send packet to a specific peer
  virtual bool send(const uint8_t *peerMac, const MatterLikePacket &packet) = 0;

  // Set user callback for received packets
  virtual void onPacketReceived(IMatterReceiver *receiver) = 0;
};
