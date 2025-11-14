#pragma once
#include <Arduino.h>
#include "../MatterLikeProtocol/MatterLikePacket.h"

class IMatterLikeTransport {
public:
  virtual ~IMatterLikeTransport() = default;

  using PacketCallback = void (*)(void *ctx, const MatterLikePacket &, const uint8_t *senderMac);


  // Initialize transport (radio, sockets, etc.)
  virtual bool begin() = 0;

  // Send packet to a specific peer
  virtual bool send(const uint8_t *peerMac, const MatterLikePacket &packet) = 0;

  // Set user callback for received packets
  virtual void onPacketReceived(PacketCallback callback, void *context) = 0;
};
