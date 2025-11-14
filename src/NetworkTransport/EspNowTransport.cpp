#include "EspNowTransport.h"
#include "../MatterLikeProtocol/MatterLikePacket.h"
#include <cstring>
#include <WiFi.h>

EspNowTransport::PacketCallback EspNowTransport::userCallback = nullptr;
void* EspNowTransport::userContext = nullptr;
static uint8_t MAX_ESP_NOW_FRAME = 250;

bool EspNowTransport::begin() {
    WiFi.mode(WIFI_STA); // must be in STA for ESP-NOW

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return false;
    }

    esp_now_register_recv_cb(onDataRecv);
    esp_now_register_send_cb(onDataSent);

    Serial.println("ESP-NOW initialized successfully");
    return true;
}

bool EspNowTransport::send(const uint8_t *peerMac, const MatterLikePacket &packet) {

    if (sizeof(MatterLikePacket) > MAX_ESP_NOW_FRAME) { // ESP-NOW max payload ~250B
        Serial.println("Packet too large for ESP-NOW!");
        return false;
    }

    uint8_t buffer[sizeof(MatterLikePacket)];
    memcpy(buffer, &packet, sizeof(MatterLikePacket));
   
    esp_err_t result = esp_now_send(peerMac, buffer, sizeof(MatterLikePacket));
    return (result == ESP_OK);
}


void EspNowTransport::onPacketReceived(PacketCallback callback, void *context) {
    userCallback  = callback;
    userContext   = context;
}

void EspNowTransport::onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (!userCallback) return;

    MatterLikePacket pkt;
    if (len > sizeof(MatterLikePacket)) {
        Serial.printf("Invalid ESP-NOW packet received (%d bytes)\n", len);
        return;
    }

    userCallback(userContext, pkt, info->src_addr);
}

void EspNowTransport::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.printf("ESP-NOW send status: %s\n",
                  status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}
