/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 13/03/2019                                           */
/*   Module: daman_gps   File: daman_gps.cpp                    */
/*   Brief:                                                     */
/****************************************************************/


/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

#include <TinyGPS++.h>

/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <daman_gps.h>
#include <dahal_sser.h>

/******************************************************************/
/*                     Definition of objects                      */
/******************************************************************/

TinyGPSPlus Gps;

/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/




/*****************************************************************/
/*            Typedef of structures and enumerations             */
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

gpsInterface_t GpsHandle (gpsData_t *gpsDataOutput)
{
    gpsInterface_t returnAnswer = NO_DATA;
    while (DahalSserAvailableData())
    {
    if (Gps.encode(DahalSserRead()))
    {
      if (Gps.location.isValid())
      {
        returnAnswer = NEW_DATA_AND_TIME;
        gpsDataOutput->latitude = Gps.location.lat();
        //Serial.print("Latitud:");
        //Serial.println(String(latitude , 6));
        gpsDataOutput->longitude = Gps.location.lng();
        //Serial.print("Longitud:");
        //Serial.println(String(longitude , 6));
      }
      if (Gps.time.isValid())
      {
        returnAnswer = (returnAnswer == NO_DATA)?NEW_TIME:returnAnswer;
        gpsDataOutput->hour = Gps.time.hour();
        gpsDataOutput->minute = Gps.time.minute();
        gpsDataOutput->second = Gps.time.second();
        gpsDataOutput->centisecond = Gps.time.centisecond();
      }
      gpsDataOutput->satellites = (uint8_t)Gps.satellites.value();
    }
  }
}

