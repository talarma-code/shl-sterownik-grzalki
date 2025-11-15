#include "EspNowTransport.h"
#include "../MatterLikeProtocol/MatterLikePacket.h"
#include <cstring>
#include <WiFi.h>


static uint8_t MAX_ESP_NOW_FRAME = 250;

IMatterReceiver* EspNowTransport::userReceiver = nullptr;


bool EspNowTransport::begin() {
    WiFi.mode(WIFI_STA); // must be in STA for ESP-NOW

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return false;
    }

    const uint8_t MAC_SHL_CENTRALLA[] = { 0x40, 0x91, 0x51, 0x20, 0xC1, 0x98 };

    if (!esp_now_is_peer_exist(MAC_SHL_CENTRALLA)) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, MAC_SHL_CENTRALLA, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);

        Serial.println("Peer auto-added!");
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


void EspNowTransport::onPacketReceived(IMatterReceiver *receiver) {
    userReceiver = receiver;
}

void EspNowTransport::onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {

    Serial.println("EspNowTransport::onDataRecv");
    if (!userReceiver) {
        Serial.printf("EspNowTransport - userReceiver NOT REGISTERED!!!\n");
        return;
    }

    if (len != sizeof(MatterLikePacket)) {
        Serial.printf("Invalid ESP-NOW packet size: %d bytes (expected %u)\n",
                      len, sizeof(MatterLikePacket));
        return;
    }

    MatterLikePacket pkt;
    memcpy(&pkt, data, sizeof(MatterLikePacket));

    userReceiver->handlePacket(pkt, info->src_addr);
}


void EspNowTransport::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.printf("ESP-NOW send status: %s\n",
                  status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}
