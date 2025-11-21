#include "EspNowTransport.h"
#include "MatterLikePacket.h"
#include <cstring>
#include <WiFi.h>
#include <esp_wifi.h>


static uint8_t MAX_ESP_NOW_FRAME = 250;

IMatterReceiver* EspNowTransport::userReceiver = nullptr;
static const uint8_t MAC_LOCAL_HEATER[]  = {0x74, 0x61, 0x6C, 0x61, 0x72, 0x31}; // talar1 - heater
static const uint8_t MAC_CENTRALKA[]   = {0x74, 0x61, 0x6C, 0x61, 0x72, 0x30}; // talar0 - centrala


bool EspNowTransport::begin() {
    delay(300);

    Serial.println("\n=== SHL Heater Controller Start ===");

    WiFi.mode(WIFI_STA);

    // set MAC address for this device (NEW: esp_wifi_set_mac requiered seperate include)
    if (esp_wifi_set_mac(WIFI_IF_STA, (uint8_t*)MAC_LOCAL_HEATER) != ESP_OK) {
        Serial.println("⚠ Could not set custom MAC!");
    }

    Serial.print("Local MAC set to: ");
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
                   MAC_LOCAL_HEATER[0], MAC_LOCAL_HEATER[1], MAC_LOCAL_HEATER[2],
                   MAC_LOCAL_HEATER[3], MAC_LOCAL_HEATER[4], MAC_LOCAL_HEATER[5]);

    // Start ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("❌ ESP-NOW init failed!");
        return false;
    }

    // Add reciver (heater) as PEER
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, MAC_CENTRALKA, 6);
    peerInfo.channel = 0;  // the same channel as in WiFi
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        Serial.println("Peer added OK");
    } else {
        Serial.println("❌ Peer add FAILED!");
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
