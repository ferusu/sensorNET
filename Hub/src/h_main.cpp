/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: Hub                                            */
/*   Author: Ferusu                                             */
/*   Date: 26/03/2019                                           */
/*   Module: main   File: h_main.cpp                            */
/*   Brief: Main module of the project, contains the main       */
/*   function and setup function.                               */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/
#include <ESP8266WiFi.h>
/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/
#include "h_global_types.h"
#include "h_main.h"
#include "hhal_tmr.h"
#include "hman_wifi.h"
#include "happ_stat.h"
/*****************************************************************/
/*          Definition of symbolic constants and macros          */
/*****************************************************************/
#define EVENTS_AMOUNT 10
#define NEXT_EVENT(A)   (A==9)?(0):(A++)
/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/
static events_t eventQueue[EVENTS_AMOUNT];
static int tailIndex = 0;
static int headIndex = 0;
static int queuedEvents = 0;
static packet_t packet;
static orderPacket_t orderPacket;
static bool newOrder;
/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/
void GpioInitialize (void);
void CheckNumberOfDataAcquirers (void);
void HSendSerialPacket (void);
void HTransmitSerialCommandAnswer (void);
/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/
void GpioInitialize (void)
{
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
}

void CheckNumberOfDataAcquirers (void)
{
  uint8_t numberOfDataAcquirers = 0;
  numberOfDataAcquirers = HNumberOfDataAcquirers();
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

void HSendSerialPacket (void)
{
    const uint16_t AccelScaleFactor = 1670;
    const uint16_t GyroScaleFactor = 131;

    int imuIndex;

    Serial.print(packet.id);
    Serial.print(";");
    Serial.print(packet.timestamp);
    Serial.println(";");
    Serial.print(packet.ggaFrame);
    Serial.println(";");
    Serial.print(packet.rmcFrame);
    Serial.println(";");
    for (imuIndex=0;imuIndex<6;imuIndex++)
    {
      Serial.print((double)(*((&packet.accelX)+imuIndex))/((imuIndex<=2)?AccelScaleFactor:GyroScaleFactor));
      Serial.print(";");
    }
    Serial.println("-");
}

//void timeFormat (void)
//{
//  static char* format[4]={"%i:","%i:","%i.","%i"};
//  int i; /* timeValue */
//  char stringTimeValue[4];
//  int timeIndex;
//    for (timeIndex=0;timeIndex<4;timeIndex++)
//  {
//    i=(int)*((&/*First time value inside a unparsed time structure*/)+timeIndex);
//    sprintf(stringTimeValue, format[timeIndex], i);
//    Serial.print(stringTimeValue);
//  }
//}

void HTransmitSerialCommandAnswer (void)
{
    Serial.print(orderPacket.incomingId);
    Serial.print(";");
    Serial.print(orderPacket.chargeLevel);
    Serial.print(";");
    Serial.print(orderPacket.wifiStrenght);
    Serial.print(";");
    Serial.print(orderPacket.imuAccelConfig);
    Serial.print(";");
    Serial.print(orderPacket.imuGyroConfig);
    Serial.println(";");
}
/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/
bool HGetEventFromQueue (events_t *event)
{
    bool isThereEvent = false;
    if (queuedEvents > 0)
    {
        tailIndex = NEXT_EVENT(tailIndex);
        *event = eventQueue[tailIndex];
        queuedEvents--;
    }
    return isThereEvent;
}

void HPutEventInQueue (events_t event)
{
    if(queuedEvents<10)
    {
        headIndex = NEXT_EVENT(headIndex);
        eventQueue[headIndex];
        queuedEvents++;
    }
}

void HubInit (void)
{
    Serial.begin(115200);
    Serial.println();
    GpioInitialize();
    HWifiInit();
    HTimerInit();
}

void HPolling (void)
{
    switch(HUdpPolling (&orderPacket, &packet))
    {
        case DATA_MESSAGE:
            HPutEventInQueue(WIFI_MESSAGE);
        break;
        case COMMAND_MESSAGE:
            HPutEventInQueue(WIFI_COMMAND);
        break;
        case NOT_MESSAGE:
        default:    
        break;
    }
    if(Serial.available())
    {
        HPutEventInQueue(UART);
    }
}

void HPeriodicEvent (void)
{
    packet.timestamp++;
    HStatPeriodicExecution (&orderPacket, &packet);
    CheckNumberOfDataAcquirers();
}

void HDataWifiEvent (void)
{
    HSendSerialPacket();
}

void HCommandWifiEvent (void)
{
    HTransmitSerialCommandAnswer();
}

void HUartEvent (void)
{
    /* Debuggin command orders place to interact */
    switch(Serial.read())
    {
        case 'a':
        /* Order 'a' */
        break;
        case 'b':
        /* Order 'b' */
        break;
    }
    HStatCommandChange(SEND_COMMAND_DATA_COLLECTER);
}