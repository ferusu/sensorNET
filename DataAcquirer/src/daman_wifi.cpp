#include "daman_wifi.h"
#include "global_types.h"
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>

/** Udp object, gives access to the Udp functions */
WiFiUDP Udp;

/** IP Address object */
IPAddress remoteIP(192, 168, 1, 200);
IPAddress localIp(192, 168, 1, 201);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char *ssid = "SensorNET";
const char *password = "123456789";
const unsigned int localPort = 2100;      // local port
const unsigned int remotePort = 2000;

static int packetSize = 0;
char packetBuffer[255]; //buffer to hold outgoing packet

void WifiInit(void)
{
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.config(localIp, gateway, subnet);
    WiFi.begin(ssid, password);
    pinMode(D4, OUTPUT);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(200);
      digitalWrite(D4, !digitalRead(D4));
    }
    digitalWrite(D4, HIGH);
    Serial.print("Conectando a:\t");
    Serial.println(ssid);
    Udp.begin(localPort);
}

bool UdpPolling (orderPacket_t *orderPacket)
{
  bool result = false;
  int len;
  packetSize = Udp.parsePacket();
  if (packetSize)
  {
    result = true;
    len = Udp.read((char *)orderPacket, sizeof(orderPacket_t));
  }
  len = 0;
  return result;
}

void SendBufferUdp (packet_t *packet)
{
  memcpy(packetBuffer, packet, sizeof(packet_t));
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write(packetBuffer,sizeof(packet_t));
  Udp.endPacket();
}

void SendCommandAnswer (orderPacket_t *orderPacket)
{
  memcpy(packetBuffer, orderPacket, sizeof(orderPacket_t));
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write(packetBuffer,sizeof(orderPacket_t));
  Udp.endPacket();
}

int32_t GetSignalStrenght (void)
{
  return WiFi.RSSI();
}