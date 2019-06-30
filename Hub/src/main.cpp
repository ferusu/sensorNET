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
#include "h_main.h"
#include "h_global_types.h"
/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/

/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/
events_t event;
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
  HubInit();
}

void loop()
{
  HPolling();
  if(HGetEventFromQueue(&event))
  {
    switch(event)
    {
      case HEARTBEAT:
        HPeriodicEvent();
      break;
      case WIFI_MESSAGE:
        HDataWifiEvent();
      break;
      case WIFI_COMMAND:
        HCommandWifiEvent();
      break;
      case UART:
        HUartEvent();
      break;
      default:
      break;
    }
  }
}