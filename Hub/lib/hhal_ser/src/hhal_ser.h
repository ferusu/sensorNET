/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: Hub                                            */
/*   Author: Ferusu                                             */
/*   Date: 21/03/2019                                           */
/*   Module: hhal_ser  File: hhal_ser.h                         */
/*   Brief:                                                     */
/****************************************************************/

#ifndef hhal_ser_h
#define hhal_ser_h

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

#include <ESP8266WiFi.h>

/*****************************************************************/
/*         Typedef of exported structures and enumerations       */
/*****************************************************************/



/*****************************************************************/
/*                  Public Function Prototypes                   */
/*****************************************************************/

void HhalSerInit(void);
bool HhalSerAvailableData(void);
char HhalSerRead (void);

#endif