#include "hman_wifi.h"
#include "h_global_types.h"
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>

/** Udp object, gives access to the Udp functions */
IPAddress ip0;
WiFiUDP Udp;

/** IP Address object */
IPAddress ip(192, 168, 0, 177);
IPAddress remoteIP(192, 168, 1, 201);
IPAddress localIp(192, 168, 1, 200);     
IPAddress gateway(192, 168, 1, 1);   
IPAddress subnet(255, 255, 255, 0);

const char *ssid = "SensorNET";
const char *password = "123456789";
const unsigned int remotePort = 2100;

static unsigned int localPort = 2000;      // local port to listen on
static packet_t packetBuffer; //buffer to hold incoming packet
static int packetSize = 0; 

bool HWifiInit(void)
{
  bool result = false;
    WiFi.disconnect();
    result = WiFi.softAP(ssid, password);
    WiFi.softAPConfig(localIp, gateway, subnet);
    Udp.begin(localPort);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());
    digitalWrite(D4, HIGH);
    return result;
}

messageType_t HUdpPolling (orderPacket_t *orderPacket, packet_t *packet)
{
  messageType_t result = NOT_MESSAGE;
  int len;
  packetSize = Udp.parsePacket();
  if (packetSize == sizeof(orderPacket_t))
  {
    result = COMMAND_MESSAGE;
    len = Udp.read((char *)orderPacket, sizeof(orderPacket_t));
  }
  else if (packetSize)
  {
    result = DATA_MESSAGE;
    len = Udp.read((char *)packet, sizeof(packet_t));
  }
  
  len = 0;
  return result;
}

void HTransmitCommand (orderPacket_t *orderPacket)
{
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write((char *)orderPacket,sizeof(orderPacket_t));
  Udp.endPacket();
}

uint8_t HNumberOfDataAcquirers (void)
{
  return WiFi.softAPgetStationNum();
}
