/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: Hub                                            */
/*   Author: Ferusu                                             */
/*   Date: 21/03/2019                                           */
/*   Module: hhal_tmr  File: hhal_tmr.h                         */
/*   Brief:                                                     */
/****************************************************************/

#ifndef dahal_tmr_h
#define dahal_tmr_h

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

#include <ESP8266WiFi.h>
#include <hhal_evt.h>

/*****************************************************************/
/*         Typedef of exported structures and enumerations       */
/*****************************************************************/



/*****************************************************************/
/*                  Public Function Prototypes                   */
/*****************************************************************/

void HhalTmrInit (void);
void HhalTmrSet (timerType_t timerType, uint16_t period);

#endif