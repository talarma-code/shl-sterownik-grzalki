#pragma once
#include "MatterLikePacket.h"

class IMatterReceiver {
public:
    virtual void handlePacket(const MatterLikePacket &pkt, const uint8_t *srcMac) = 0;
    virtual ~IMatterReceiver() = default;
};
