#ifndef h_main_h
#define h_main_h

typedef enum
{
    HEARTBEAT,
    WIFI_MESSAGE,
    WIFI_COMMAND,
    UART
}events_t;

bool HGetEventFromQueue (events_t *event);
void HPutEventInQueue (events_t event);
void HubInit (void);
void HPolling (void);
void HPeriodicEvent (void);
void HDataWifiEvent (void);
void HCommandWifiEvent (void);
void HUartEvent (void);

#endif