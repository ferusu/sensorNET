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
#define NEXT_EVENT(A)   (A==9)?(0):(A++) //Macro function to move along a circular buffer
/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/
static events_t eventQueue[EVENTS_AMOUNT]; //Event queue array
static int tailIndex = 0; //Tail index points to the event queue that is gonna be attended
static int headIndex = 0; //Head index points to the last event queued
static int queuedEvents = 0; //Queued events store the number of events stored in the queue
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
    if (queuedEvents > 0) //Check if there is any new event queued
    {
        tailIndex = NEXT_EVENT(tailIndex); //Update the tail index
        *event = eventQueue[tailIndex]; //Return through the referenced parameter the event information
        queuedEvents--; //Substrac event from the queued events counter
        isThereEvent = true; //Set as true the return value, so the function 
        //answers that there is a new event to attend
    }
    return isThereEvent; //Return answer value
}

void PutEventInQueue (events_t event)
{
    if(queuedEvents<EVENTS_AMOUNT) //Check if the number of events does not exceed the lenght of 
    //the queue
    {
        headIndex = NEXT_EVENT(headIndex); //Update the head index
        eventQueue[headIndex] = event; //Store the event in the event queue
        queuedEvents++; //Add event to the queued events counter
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