#ifndef daman_wifi_h
#define daman_wifi_h


void WifiInit(void);
bool UdpPolling (orderPacket_t *orderPacket);
void SendBufferUdp (packet_t *packet);
void SendCommandAnswer (orderPacket_t *orderPacket);
#endif