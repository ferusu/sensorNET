/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 21/03/2019                                           */
/*   Module: hhal_tmr   File: hhal_tmr.cpp                      */
/*   Brief:                                                     */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <hhal_tmr.h>

/******************************************************************/
/*                     Definition of objects                      */
/******************************************************************/



/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/



/******************************************************************/
/*           Definition of local functions like macros            */
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

os_timer_t softwareTimer;
static bool timerInit = false;
static uint16_t prescaler;
static event_t event;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

void timerCallback(void *pArg);


/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

void timerCallback(void *pArg)
{
    static uint32_t mainStatMachineTimestamp = 0;
    static uint32_t heartbeatTimestamp = 0;
    static uint16_t prescalerCounter;

    event.timer.timerType = HEARTBEAT_TIMER;
    event.timer.timestamp = mainStatMachineTimestamp;
    HhalEvtQueuePut (event);
    heartbeatTimestamp++;
    prescalerCounter++;
    if (prescalerCounter>=prescaler)
    {
        event.timer.timerType = MAIN_STATE_MACHINE_TIMER;
        event.timer.timestamp = mainStatMachineTimestamp;
        HhalEvtQueuePut (event);
        mainStatMachineTimestamp++;
        prescalerCounter = 0;
        digitalWrite(D4, !digitalRead(D4));
    }
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void HhalTmrInit (void)
{
    os_timer_setfn(&softwareTimer, timerCallback, NULL);
    event.eventType =  DAHAL_EVENT_TYPE_TIMER;
    timerInit = true;
}

void HhalTmrSet (timerType_t timerType, uint16_t period)
{
    static uint16_t mainStateMachinePeriod = 0;
    static uint16_t heartbeatPeriod = 0;
    if(timerInit)
    {
        switch(timerType)
        {
            case MAIN_STATE_MACHINE_TIMER:
                if ((period>heartbeatPeriod)&&(heartbeatPeriod>0))
                {
                    prescaler = period/heartbeatPeriod;
                    mainStateMachinePeriod = heartbeatPeriod*prescaler;
                    Serial.println(mainStateMachinePeriod);
                }
                break;
            case HEARTBEAT_TIMER:
                heartbeatPeriod = period;
                Serial.println(heartbeatPeriod);
                break;
        }
        os_timer_arm(&softwareTimer, (int)heartbeatPeriod, true);
    }
}