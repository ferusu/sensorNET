#include <dahal_wifi.h>

/** Udp object, gives access to the Udp functions */
WiFiUDP Udp;

const char *ssid = "Budhahouse";
const char *password = "corbalmasid";
const unsigned int remotePort = 2000;      // local port to listen on

IPAddress remoteIP(192, 168, 0, 32);
IPAddress localIp(192, 168, 0, 201);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

static bool wifiInitialized = false;
static bool udpInitialized = false;
//static char packetBuffer[24]; //buffer to hold incoming packet
static udpData_t udpDataLocal;

void DahalWifiInit (void)
{
    if (wifiInitialized == false)
    {
        WiFi.mode(WIFI_STA);
        //WiFi.config(localIp, gateway, subnet);
        WiFi.begin(ssid, password);
        Serial.print("Conectando a:\t");
//        while (WiFi.status() != WL_CONNECTED) 
//        {
//            delay(200); 
//            Serial.print('.');
//        }
//        Serial.println(ssid);
        wifiInitialized = true;
    }
}

wl_status_t DahalWifiStatus (void)
{
    wl_status_t returnAnswer = WiFi.status();
    return returnAnswer;
}

bool DahalWifiUdpInit (unsigned int localPort)
{
    bool returnAnswer = false;
    if (wifiInitialized)
    {
        returnAnswer = (bool)Udp.begin(localPort);
        udpInitialized = true;
    }
    return returnAnswer;
}

void DahalWifiSend(char *outgoingUdpPacket)
{
    if (udpInitialized)
    {
        Udp.beginPacket(remoteIP, remotePort);
        Udp.write(outgoingUdpPacket);
        Udp.endPacket();
    }
}

bool DahalWifiAvailableData(void)
{
    bool returnValue = false;
    int length = 0;
    if (udpInitialized)
    {
        length = Udp.parsePacket();
        returnValue = (length != 0)?true:false;
    }
    return returnValue;
}

bool DahalWifiRead(udpData_t *udpData)
{
    int length;
    bool returnAnswer = false;
    if (udpInitialized)
    {
        length = Udp.read(udpData->udpPacket, 24);
        returnAnswer = (length>24)?false:true;
        udpData->remoteIp = Udp.remoteIP();
        udpData->remotePort = Udp.remotePort();
        udpData->packetSize = length;
    }
    return returnAnswer;
}

uint8_t DahalWifiCrc (char *packet, int packetSize)
{
    uint8_t returnAnswer = 0;
    int packetIndex = 0;
    returnAnswer = (uint8_t)(*packet);
    /* Menor que (sin "o igual") porque se parte de cero: size = 8, index[0-7]
    y menos dos porque el penúltimo dato es el crc con el que se va a comparar
    y el último dato es un '/0' que indica el final de un string. packetIndex
    empieza en uno porque el primer valor lo metemos antes del for*/
    for (packetIndex = 1; packetIndex<(packetSize-2); packetIndex++)
    {
        returnAnswer ^= *(packet+packetIndex);
    }
    return returnAnswer;
}
