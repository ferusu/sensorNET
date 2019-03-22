/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 21/03/2019                                           */
/*   Module: hhal_evt   File: hhal_evt.cpp                      */
/*   Brief:                                                     */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <hhal_evt.h>
#include <hhal_ser.h>

/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/

#define BUFFER_LENGHT                   (10U)

/******************************************************************/
/*           Definition of local functions like macros            */
/******************************************************************/

#define HANDLE_HEAD(A)  (A>=(BUFFER_LENGHT))?0:(A+1)
#define HANDLE_TAIL(A)  (A>=(BUFFER_LENGHT))?0:(A+1)

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/

typedef enum
{
    ADD_EVENT = 0,
    GET_EVENT
}orderType_t;

/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/

const event_t serialEvent = {
    .eventType = DAHAL_EVENT_TYPE_SERIAL,
    .timer = {
        .timerType = (timerType_t)0,
        .timestamp = (uint32_t)0U }};

const event_t wifiEvent = {
    .eventType = DAHAL_EVENT_TYPE_WIFI,
    .timer = {
        .timerType = (timerType_t)0,
        .timestamp = (uint32_t)0U }};

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

static event_t eventBuffer[BUFFER_LENGHT];
static uint8_t eventIndex = 0;
static uint8_t headIndex = 0;
static uint8_t tailIndex = 0;
static bool hhalEvtInitialized = false;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

eventHandlingResult_t EventQueueHandler(orderType_t orderType, event_t *eventOutput);
void EventSoftwareSerialPolling (void);
void EventWifiPolling (void);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

eventHandlingResult_t EventQueueHandler(orderType_t orderType, event_t *eventOutput)
{
    eventHandlingResult_t returnValue = ERROR;
    if(hhalEvtInitialized)
    {
        switch (orderType)
        {
            case ADD_EVENT:
                if (eventIndex < (BUFFER_LENGHT))
                {
                    switch (eventOutput->eventType)
                    {
                        case DAHAL_EVENT_TYPE_WIFI:
                            eventBuffer[headIndex].eventType =  DAHAL_EVENT_TYPE_WIFI;
                            returnValue = SUCCESS;
                            break;
                        case DAHAL_EVENT_TYPE_SERIAL:
                            eventBuffer[headIndex].eventType =  DAHAL_EVENT_TYPE_SERIAL;
                            returnValue = SUCCESS;
                            break;
                        case DAHAL_EVENT_TYPE_TIMER:
                            eventBuffer[headIndex].eventType =  DAHAL_EVENT_TYPE_TIMER;
                            eventBuffer[headIndex].timer.timerType = eventOutput->timer.timerType;
                            eventBuffer[headIndex].timer.timestamp = eventOutput->timer.timestamp;
                            returnValue = SUCCESS;
                            break;
                        default:
                            returnValue = NOT_KNOWN_EVENT_TYPE;
                            break;
                    }
                    if(returnValue == SUCCESS)
                    {
                        eventIndex++;
                        headIndex = HANDLE_HEAD(headIndex);
                    }
                }
                else
                {
                    returnValue = EVENT_QUEUE_OVERLOAD;
                }
                
                break;
            case GET_EVENT:
                if (eventIndex>0)
                {
                    eventIndex--;
                    eventOutput->eventType = eventBuffer[tailIndex].eventType;
                    eventOutput->timer.timerType = eventBuffer[tailIndex].timer.timerType;
                    eventOutput->timer.timestamp = eventBuffer[tailIndex].timer.timestamp;
                    returnValue = SUCCESS;
                    tailIndex = HANDLE_TAIL(tailIndex);
                }
                else
                {
                    returnValue = NOT_EVENT;
                }                
                break;
            default:
                    returnValue = NOT_KNOWN_ORDER;
                break;
        }
    }
    return returnValue;
}

void EventSoftwareSerialPolling (void)
{
    if(HhalSerAvailableData())
    {
        EventQueueHandler(ADD_EVENT, (event_t *)&serialEvent);
    }
}

void EventWifiPolling (void)
{
//    if(DahalWifiAvailableData())
    {
        EventQueueHandler(ADD_EVENT, (event_t *)&wifiEvent);
    }
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void HhalEvtInit(void)
{
    eventIndex = 0;
    hhalEvtInitialized = true;
}

bool DahalEvtGet(event_t *eventOutput)
{
    EventSoftwareSerialPolling();
    EventWifiPolling();
    return (EventQueueHandler(GET_EVENT, eventOutput)==SUCCESS)?true:false;
}

eventHandlingResult_t HhalEvtQueuePut (event_t eventInput)
{
    return EventQueueHandler(ADD_EVENT, &eventInput);
}