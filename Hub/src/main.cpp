#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>
IPAddress ip0;
WiFiUDP Udp;
unsigned int localPort = 2399;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "papuda";       // a string to send back
os_timer_t myTimer;

IPAddress ip(192, 168, 0, 177);
IPAddress ip1(0, 0, 0, 0);
IPAddress ip2(0, 0, 0, 0);
IPAddress ip3(0, 0, 0, 0);
IPAddress ip4(0, 0, 0, 0);
IPAddress remoteIP(192, 168, 4, 2);

void setup()
{
  Serial.begin(9600);
  Serial.println();
  WiFi.disconnect(); 
  WiFi.config(ip, ip1, ip2, ip3, ip4);
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("SensorNET", "123456789") ? "Ready" : "Failed!");
  Udp.begin(localPort);
  pinMode(D4, OUTPUT);
}

void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    digitalWrite(D4, !digitalRead(D4));

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
  if((millis()&0xFFF)==3000)
  {
    Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
    ip = WiFi.localIP();
    Serial.println(ip);
    Udp.beginPacket(remoteIP, 62308);
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}