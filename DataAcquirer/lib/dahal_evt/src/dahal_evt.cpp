#include <dahal_evt.h>

#define BUFFER_LENGHT                   (10U)
#define BUFFER_OFFSET                   (2U)           /* Designed to add high priority events */
#define MAX_INTERRUPT_PRIORITY          (0U)
#define SECOND_INTERRUPT_PRIORITY       (1U)

typedef enum
{
    ADD_EVENT = 0,
    GET_EVENT
}orderType_t;

static event_t eventBuffer[BUFFER_LENGHT+BUFFER_OFFSET];
static uint8_t eventIndex = 0;

eventHandlingResult_t EventQueueHandler(orderType_t orderType, event_t *eventOutput);

eventHandlingResult_t EventQueueHandler(orderType_t orderType, event_t *eventOutput)
{
    eventHandlingResult_t returnValue = ERROR;
    switch (orderType)
    {
        case ADD_EVENT:
            if (eventIndex < (BUFFER_LENGHT+BUFFER_OFFSET))
            {
                switch (eventOutput->enventType)
                {
                    case DAHAL_EVENT_TYPE_WIFI:
                        eventBuffer[eventIndex].enventType =  DAHAL_EVENT_TYPE_WIFI;
                        returnValue = SUCCESS;
                        break;
                    case DAHAL_EVENT_TYPE_SOFTWARE_SERIAL:
                        eventBuffer[eventIndex].enventType =  DAHAL_EVENT_TYPE_SOFTWARE_SERIAL;
                        returnValue = SUCCESS;
                        break;
                    case DAHAL_EVENT_TYPE_TIMER:
                        eventBuffer[eventIndex].enventType =  DAHAL_EVENT_TYPE_TIMER;
                        eventBuffer[eventIndex].timer.timerType = eventOutput->timer.timerType;
                        eventBuffer[eventIndex].timer.timestamp = eventOutput->timer.timestamp;
                        returnValue = SUCCESS;
                        break;
                    default:
                        returnValue = NOT_KNOWN_EVENT_TYPE;
                        break;
                }
                eventIndex = (returnValue == SUCCESS)?eventIndex++:eventIndex;
            }
            else
            {
                returnValue = EVENT_QUEUE_OVERLOAD;
            }
            
            break;
        case GET_EVENT:
            if (eventBuffer[MAX_INTERRUPT_PRIORITY].enventType != DAHAL_EVENT_TYPE_NOT_EVENT)
            {
                /* Return from here max priority interrupt event. It will be returned before the 
                others, keeping them on hold */

                eventBuffer[MAX_INTERRUPT_PRIORITY].enventType = DAHAL_EVENT_TYPE_NOT_EVENT;
                returnValue = SUCCESS;
            }
            else if (eventBuffer[SECOND_INTERRUPT_PRIORITY].enventType != DAHAL_EVENT_TYPE_NOT_EVENT)
            {
                /* Return from here second priority interrupt event. It will be returned before the 
                third priority events, keeping them on hold */

                eventBuffer[SECOND_INTERRUPT_PRIORITY].enventType = DAHAL_EVENT_TYPE_NOT_EVENT;
                returnValue = SUCCESS;
            }
            else if (eventIndex>BUFFER_OFFSET)
            {
                eventIndex--;
                memcpy(eventOutput,(eventBuffer+eventIndex),sizeof(event_t));
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
    return returnValue;
}

DahalEvt::DahalEvt()
{
    eventBuffer[SECOND_INTERRUPT_PRIORITY].enventType = DAHAL_EVENT_TYPE_NOT_EVENT;
    eventBuffer[SECOND_INTERRUPT_PRIORITY].enventType = DAHAL_EVENT_TYPE_NOT_EVENT;
}

bool DahalEvt::eventGet(event_t *eventOutput)
{
return (EventQueueHandler(GET_EVENT, eventOutput)==SUCCESS)?true:false;
}


void DahalEvt::softwareSerialPolling (void)
{
    const event_t sserialEvent = {
        .enventType = DAHAL_EVENT_TYPE_SOFTWARE_SERIAL,
        .timer.timerType = (timerType_t)0,
        .timer.timestamp = 0U };

    if(/* Call software serial interface */NULL)
    {
        EventQueueHandler(ADD_EVENT, (event_t *)&sserialEvent);
    }
}

void DahalEvt::wifiPolling (void)
{
    const event_t wifiEvent = {
        .enventType = DAHAL_EVENT_TYPE_WIFI,
        .timer.timerType = (timerType_t)0,
        .timer.timestamp = 0U };

    if(/* Call software serial interface */NULL)
    {
        EventQueueHandler(ADD_EVENT, (event_t *)&wifiEvent);
    }
}

eventHandlingResult_t EventQueuePut (event_t eventInput)
{
    return EventQueueHandler(ADD_EVENT, &eventInput);
}

