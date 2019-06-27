#ifndef da_main_h
#define da_main_h

typedef enum
{
    HEARTBEAT,
    WIFI,
    UART
}events_t;

bool GetEventFromQueue (events_t *event);
void PutEventInQueue (events_t event);





#endif