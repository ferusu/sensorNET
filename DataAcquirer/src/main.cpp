/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 25/02/2019                                           */
/*   Module: main   File: main.cpp                              */
/*   Brief: Main module of the project, contains the main       */
/*   function and setup function.                               */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

#include <ESP8266WiFi.h>        // Include the Wi-Fi library

/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <dahal_evt.h>
#include <dahal.h>
#include <dahal_sser.h>
#include <daman_gps.h>
#include <daapp_stat.h>
#include <daman_net.h>

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/



/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/



/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/



/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

static event_t event;
static gpsData_t gpsData;
static gpsInterface_t gpsState;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/



/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/



/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void setup()
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("Init");
  DahalEvtInit();
  Serial.println("Event initialized");
  DahalInit();
  Serial.println("Hal initialized");
  pinMode(D4, OUTPUT);
  Serial.println("Gpio initialized");
  DaappStatInit (&gpsData);
}

void loop()
{
  if(DahalEvtGet(&event))
  {
    switch(event.eventType)
    {
      case DAHAL_EVENT_TYPE_NOT_EVENT:
        
        break;
      case DAHAL_EVENT_TYPE_WIFI:
        DamanNetDiggestPacket();
        break;
      case DAHAL_EVENT_TYPE_SOFTWARE_SERIAL:
        gpsState = GpsHandle(&gpsData);
        break;
      case DAHAL_EVENT_TYPE_TIMER:
        if(event.timer.timerType == MAIN_STATE_MACHINE_TIMER)
        {
          DaappStatFirstTimeBase();
        }
        if(event.timer.timerType == HEARTBEAT_TIMER)
        {
          DaappStatSecondTimeBase(gpsState);
        }
        break;
      default:
          Serial.print('Y');
        break;
    }
  }
}

