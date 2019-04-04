/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: Hub                                            */
/*   Author: Ferusu                                             */
/*   Date: 26/03/2019                                           */
/*   Module: main   File: main.cpp                              */
/*   Brief: Main module of the project, contains the main       */
/*   function and setup function.                               */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>

/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/

IPAddress ip0;
WiFiUDP Udp;
os_timer_t softwareTimer;
IPAddress ip(192, 168, 0, 177);
IPAddress remoteIP(192, 168, 1, 201);
IPAddress localIp(192, 168, 1, 200);     
IPAddress gateway(192, 168, 1, 1);   
IPAddress subnet(255, 255, 255, 0);

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/

typedef struct
{
  /* Start of the packet */
  char id;
  char id2;
  char id3;
  char id4;
  uint32_t timestamp;
  /* Gps variables */
  float latitudeDeg;
  float longitudeDeg;
  double courseDeg;
  double speedKmph;
  uint8_t timeHour;
  uint8_t timeMinute;
  uint8_t timeSecond;
  uint8_t timeCentisecond;
  uint8_t numberOfSatellites;
  /* Imu variables */
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
  int16_t gyroX;
  int16_t gyroY;
  int16_t gyroZ;
  /* End of string */
  char endOfString;
}packet_t;

/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/

const char *ssid = "SensorNET";
const char *password = "123456789";

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

static unsigned int localPort = 2000;      // local port to listen on
static packet_t packetBuffer; //buffer to hold incoming packet
static int packetSize = 0; 
static uint32_t heartbeatTimestamp = 0;
static bool heartbeatTick = false;
static int heartbeatPeriod = 1000;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

void TimerInit (void);
void GpioInitialize (void);
bool WifiInitialize (void);
bool UdpPolling (void);
void SendSerialPacket (void);
void CheckNumberOfDataAcquirers (void);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

void timerCallback(void *pArg)
{
  heartbeatTimestamp++;
  heartbeatTick = true;
}

void TimerInit (void)
{
  os_timer_setfn(&softwareTimer, timerCallback, NULL);
  os_timer_arm(&softwareTimer, (int)heartbeatPeriod, true);
}

void GpioInitialize (void)
{
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
}

bool WifiInitialize (void)
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

bool UdpPolling (void)
{
  bool result = false;
  int len;
  packetSize = Udp.parsePacket();
  if (packetSize)
  {
    result = true;
    len = Udp.read((char *)&packetBuffer, sizeof(packet_t));
  }
  return result;
}

void SendSerialPacket (void)
{
  const uint16_t AccelScaleFactor = 16384;
  const uint16_t GyroScaleFactor = 131;
  static char* format[4]={"%i:","%i:","%i.","%i"};
  int i; /* timeValue */
  char stringTimeValue[4];
  int timeIndex;
  int imuIndex;
  char sendFloat[8];
  Serial.print(packetBuffer.id);
  Serial.print(";");
  Serial.print(packetBuffer.timestamp);
  Serial.print(";");
  Serial.print(dtostrf(packetBuffer.latitudeDeg, 3, 4, sendFloat));
  Serial.print(";");
  Serial.print(dtostrf(packetBuffer.longitudeDeg, 3, 4, sendFloat));
  Serial.print(";");
  for (timeIndex=0;timeIndex<4;timeIndex++)
  {
    i=(int)*((&packetBuffer.timeHour)+timeIndex);
    sprintf(stringTimeValue, format[timeIndex], i);
    Serial.print(stringTimeValue);
  }
  Serial.print(";");
  Serial.print(packetBuffer.courseDeg);
  Serial.print(";");
  Serial.print(packetBuffer.speedKmph);
  Serial.print(";");
  for (imuIndex=0;imuIndex<6;imuIndex++)
  {
    Serial.print((double)(*((&packetBuffer.accelX)+imuIndex))/((imuIndex>2)?AccelScaleFactor:GyroScaleFactor));
    Serial.print(";");
  }
  //Ax = (double)packetBuffer.accelX/AccelScaleFactor;
  //Ay = (double)packetBuffer.accelY/AccelScaleFactor;
  //Az = (double)packetBuffer.accelZ/AccelScaleFactor;
  //Gx = (double)packetBuffer.gyroX/GyroScaleFactor;
  //Gy = (double)packetBuffer.gyroY/GyroScaleFactor;
  //Gz = (double)packetBuffer.gyroZ/GyroScaleFactor;
  //Serial.print(Ax);
  //Serial.print(Ay);
  //Serial.print(Az);
  //Serial.print(Gx);
  //Serial.print(Gy);
  //Serial.print(Gz);
Serial.println("-");
}

void CheckNumberOfDataAcquirers (void)
{
  uint8_t numberOfDataAcquirers = 0;
  numberOfDataAcquirers = WiFi.softAPgetStationNum();
  switch (numberOfDataAcquirers)
  {
    case 0:
      digitalWrite(D0, LOW);
      digitalWrite(D1, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      break;
    case 1:
      digitalWrite(D0, HIGH);
      digitalWrite(D1, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      break;
    case 2:
      digitalWrite(D0, HIGH);
      digitalWrite(D1, HIGH);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      break;
    case 3:
      digitalWrite(D0, HIGH);
      digitalWrite(D1, HIGH);
      digitalWrite(D2, HIGH);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      break;
    case 4:
      digitalWrite(D0, HIGH);
      digitalWrite(D1, HIGH);
      digitalWrite(D2, HIGH);
      digitalWrite(D3, HIGH);
      digitalWrite(D4, LOW);
      break;
    case 5:
      digitalWrite(D0, HIGH);
      digitalWrite(D1, HIGH);
      digitalWrite(D2, HIGH);
      digitalWrite(D3, HIGH);
      digitalWrite(D4, HIGH);
    default:

      break;
  }
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void setup()
{
  Serial.begin(115200);
  Serial.println();
  GpioInitialize();
  WifiInitialize();
  TimerInit ();
}

void loop()
{
  bool sendMessageFlag = false;
  sendMessageFlag = UdpPolling ();
  if (sendMessageFlag)
  {
    SendSerialPacket ();
  }
  if (heartbeatTick)
  {
    heartbeatTick = false;
    CheckNumberOfDataAcquirers();
  }
}