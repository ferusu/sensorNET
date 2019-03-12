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
}

void loop()
{
  if(DahalEvtGet(&event))
  {
    Serial.print('.');
    switch(event.eventType)
    {
      case DAHAL_EVENT_TYPE_NOT_EVENT:
        Serial.print('0');
        break;
      case DAHAL_EVENT_TYPE_WIFI:
        Serial.print('1');
        break;
      case DAHAL_EVENT_TYPE_SOFTWARE_SERIAL:
        Serial.print('2');
        while(DahalSserAvailableData())
        {
          (void)DahalSserRead();
        }
        break;
      case DAHAL_EVENT_TYPE_TIMER:
        Serial.print('3');
        if(event.timer.timerType == MAIN_STATE_MACHINE_TIMER)
        {
          Serial.println("hola");
        }
        if(event.timer.timerType == HEARTBEAT_TIMER)
        {
          Serial.println("mundo");
        }
        break;
      default:
          Serial.print('Y');
        break;
    }
  }
}

