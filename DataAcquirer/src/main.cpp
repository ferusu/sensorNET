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
  DahalEvtInit();
  DahalInit();
}

void loop()
{
  event_t event;
  if(DahalEvtGet(&event))
  {
    switch(event.eventType)
    {
      case DAHAL_EVENT_TYPE_NOT_EVENT:

        break;
      case DAHAL_EVENT_TYPE_WIFI:

        break;
      case DAHAL_EVENT_TYPE_SOFTWARE_SERIAL:

        break;
      case DAHAL_EVENT_TYPE_TIMER:
        if(event.timer.timerType == MAIN_STATE_MACHINE_TIMER)
        {

        }
        if(event.timer.timerType == HEARTBEAT_TIMER)
        {
          
        }
        break;
      default:

        break;
    }
  }
}

