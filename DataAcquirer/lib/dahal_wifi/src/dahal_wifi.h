
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

class DahalWifi
{
  public:
    DahalWifi();
    void send(char *outgoingUdpPacket);
    bool availableData(void);
    bool read(udpData_t *udpData);
    void udpInit (unsigned int localPort);
  private:
    int _pin;
};
 
#endif