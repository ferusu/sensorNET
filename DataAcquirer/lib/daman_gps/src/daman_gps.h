/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 13/03/2019                                           */
/*   Module: daman_gps   File: daman_gps.h                      */
/*   Brief:                                                     */
/****************************************************************/

#ifndef daman_gps_h
#define daman_gps_h

#include <ESP8266WiFi.h>

/*****************************************************************/
/*         Typedef of exported structures and enumerations       */
/*****************************************************************/

typedef struct
{
  float latitude;
  float longitude;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t centisecond;
}gpsData_t;

typedef enum
{
    NEW_TIME = 0,
    NEW_DATA_AND_TIME,
    NO_DATA
}gpsInterface_t;

/*****************************************************************/
/*                  Public Function Prototypes                   */
/*****************************************************************/








#endif