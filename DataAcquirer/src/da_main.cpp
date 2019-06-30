/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 25/02/2019                                           */
/*   Module: main   File: da_main.cpp                           */
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
#include "global_types.h"
#include "da_main.h"
#include "daman_gps.h"
#include "dahal_tmr.h"
#include "daman_imu.h"
#include "daman_wifi.h"
#include "daapp_stat.h"
/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/

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
void VariablesInit (void);
/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/
void VariablesInit (void)
{
    packet.id = DA_ID;
    packet.id2 = DA_ID;
    packet.id3 = DA_ID;
    packet.id4 = DA_ID;
    packet.accelX = 10;
    packet.accelY = 10;
    packet.accelZ = 10;
    packet.gyroX = 10;
    packet.gyroY = 10;
    packet.gyroZ = 10;
    packet.endOfString = '\0';
}
/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/
bool GetEventFromQueue (events_t *event)
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

void PutEventInQueue (events_t event)
{
    if(queuedEvents<10)
    {
        headIndex = NEXT_EVENT(headIndex);
        eventQueue[headIndex];
        queuedEvents++;
    }
}

void DataAcquirerInit (void)
{
    Serial.begin(9600);
    GpsInit();
    TimerInit();
    VariablesInit ();
    ImuInit();
    WifiInit();
}

void Polling (void)
{
    GpsHandle(&packet);
    if(UdpPolling (&orderPacket))
    {
        PutEventInQueue(WIFI);
    }
    if(Serial.available())
    {
        PutEventInQueue(UART);
    }
}

void PeriodicEvent (void)
{
    packet.timestamp++;
    ImuHandle(&packet);
    StatPeriodicExecution (&orderPacket, &packet);
}

void WifiEvent (void)
{
    StatCommandReceived ();
}

void UartEvent (void)
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
}