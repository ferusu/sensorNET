
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
void DahalWifiUdpInit (unsigned int localPort);
void DahalWifiSend(char *outgoingUdpPacket);
bool DahalWifiAvailableData(void);
bool DahalWifiRead(udpData_t *udpData);

 
#endif