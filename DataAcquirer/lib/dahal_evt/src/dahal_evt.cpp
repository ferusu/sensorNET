/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 25/02/2019                                           */
/*   Module: dahal_evt   File: dahal_evt.cpp                    */
/*   Brief:                                                     */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <dahal_evt.h>
#include <dahal_sser.h>
#include <dahal_wifi.h>

/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/

#define BUFFER_LENGHT                   (10U)
#define BUFFER_OFFSET                   (2U)           /* Designed to add high priority events */
#define MAX_INTERRUPT_PRIORITY          (0U)
#define SECOND_INTERRUPT_PRIORITY       (1U)

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

const event_t sserialEvent = {
    .eventType = DAHAL_EVENT_TYPE_SOFTWARE_SERIAL,
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

static event_t eventBuffer[BUFFER_LENGHT+BUFFER_OFFSET];
static uint8_t eventIndex = (uint8_t)0U;
static bool dahalEvtInitialized = false;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

eventHandlingResult_t EventQueueHandler(orderType_t orderType, event_t *eventOutput);
void DahalEvtSoftwareSerialPolling (void);
void DahalEvtWifiPolling (void);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

eventHandlingResult_t EventQueueHandler(orderType_t orderType, event_t *eventOutput)
{
    eventHandlingResult_t returnValue = ERROR;
    if(dahalEvtInitialized)
    {
        switch (orderType)
        {
            case ADD_EVENT:
                if (eventIndex < (BUFFER_LENGHT+BUFFER_OFFSET))
                {
                    switch (eventOutput->eventType)
                    {
                        case DAHAL_EVENT_TYPE_WIFI:
                            eventBuffer[eventIndex].eventType =  DAHAL_EVENT_TYPE_WIFI;
                            returnValue = SUCCESS;
                            break;
                        case DAHAL_EVENT_TYPE_SOFTWARE_SERIAL:
                            eventBuffer[eventIndex].eventType =  DAHAL_EVENT_TYPE_SOFTWARE_SERIAL;
                            returnValue = SUCCESS;
                            break;
                        case DAHAL_EVENT_TYPE_TIMER:
                            Serial.print('-');
                            eventBuffer[eventIndex].eventType =  DAHAL_EVENT_TYPE_TIMER;
                            eventBuffer[eventIndex].timer.timerType = eventOutput->timer.timerType;
                            eventBuffer[eventIndex].timer.timestamp = eventOutput->timer.timestamp;
                            returnValue = SUCCESS;
                            break;
                        default:
                            returnValue = NOT_KNOWN_EVENT_TYPE;
                            break;
                    }
                    if(returnValue == SUCCESS)
                    {
                        eventIndex++;
                    }
                }
                else
                {
                    returnValue = EVENT_QUEUE_OVERLOAD;
                }
                
                break;
            case GET_EVENT:
                if (eventBuffer[MAX_INTERRUPT_PRIORITY].eventType != DAHAL_EVENT_TYPE_NOT_EVENT)
                {
                    /* Return from here max priority interrupt event. It will be returned before the 
                    others, keeping them on hold */
    
                    eventBuffer[MAX_INTERRUPT_PRIORITY].eventType = DAHAL_EVENT_TYPE_NOT_EVENT;
                    returnValue = SUCCESS;
                }
                else if (eventBuffer[SECOND_INTERRUPT_PRIORITY].eventType != DAHAL_EVENT_TYPE_NOT_EVENT)
                {
                    /* Return from here second priority interrupt event. It will be returned before the 
                    third priority events, keeping them on hold */
    
                    eventBuffer[SECOND_INTERRUPT_PRIORITY].eventType = DAHAL_EVENT_TYPE_NOT_EVENT;
                    returnValue = SUCCESS;
                }
                else if (eventIndex>BUFFER_OFFSET)
                {
                    eventIndex--;
                    //memcpy(eventOutput,(eventBuffer+eventIndex),sizeof(event_t));
                    eventOutput->eventType = eventBuffer[eventIndex].eventType;
                    eventOutput->timer.timerType = eventBuffer[eventIndex].timer.timerType;
                    eventOutput->timer.timestamp = eventBuffer[eventIndex].timer.timestamp;
                    returnValue = SUCCESS;
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


void DahalEvtSoftwareSerialPolling (void)
{
    if(DahalSserAvailableData())
    {
        EventQueueHandler(ADD_EVENT, (event_t *)&sserialEvent);
    }
}

void DahalEvtWifiPolling (void)
{
    if(DahalWifiAvailableData())
    {
        EventQueueHandler(ADD_EVENT, (event_t *)&wifiEvent);
    }
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void DahalEvtInit(void)
{
    eventBuffer[MAX_INTERRUPT_PRIORITY].eventType = DAHAL_EVENT_TYPE_NOT_EVENT;
    eventBuffer[SECOND_INTERRUPT_PRIORITY].eventType = DAHAL_EVENT_TYPE_NOT_EVENT;
    eventIndex = 0;
    dahalEvtInitialized = true;
}

bool DahalEvtGet(event_t *eventOutput)
{
    DahalEvtSoftwareSerialPolling();
    DahalEvtWifiPolling();
    return (EventQueueHandler(GET_EVENT, eventOutput)==SUCCESS)?true:false;
}

eventHandlingResult_t DahalEvtQueuePut (event_t eventInput)
{
    return EventQueueHandler(ADD_EVENT, &eventInput);
}

