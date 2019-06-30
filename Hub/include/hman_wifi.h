#ifndef hman_wifi_h
#define hman_wifi_h


bool HWifiInit(void);
messageType_t HUdpPolling (orderPacket_t *orderPacket, packet_t *packet);
void HTransmitCommand (orderPacket_t *orderPacket);
uint8_t HNumberOfDataAcquirers (void);
#endif