
#ifndef dahal_wifi_h
#define dahal_wifi_h
 
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

typedef struct
{
    IPAddress remoteIp;
    uint16_t remotePort;
    int packetSize;
    char udpPacket[24];
}udpData_t;

void DahalWifiInit(void);
wl_status_t DahalWifiStatus (void);
bool DahalWifiUdpInit (unsigned int localPort);
void DahalWifiSend(char *outgoingUdpPacket);
bool DahalWifiAvailableData(void);
bool DahalWifiRead(udpData_t *udpData);
uint8_t DahalWifiCrc (char *packet, int packetSize);
 
#endif