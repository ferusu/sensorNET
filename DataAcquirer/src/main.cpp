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
#include <WiFiManager.h>
/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/
#include "da_main.h"
#include "global_types.h"
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
  DataAcquirerInit();
}

void loop()
{
  Polling();
  if(GetEventFromQueue(&event))
  {
    switch(event)
    {
      case HEARTBEAT:
        PeriodicEvent();
      break;
      case WIFI:
        WifiEvent();
      break;
      case UART:
        UartEvent();
      break;
      default:
        /* Unidentified event */
      break;
    }
  }
}

