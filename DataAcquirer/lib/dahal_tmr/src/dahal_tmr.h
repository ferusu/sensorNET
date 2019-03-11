#ifndef dahal_tmr_h
#define dahal_tmr_h

#include <ESP8266WiFi.h>

typedef enum
{
    MAIN_STATE_MACHINE_TIMER = 0,
    HEARTBEAT_TIMER
}timerType_t;

void DahalTmrInit (void);
void DahalTmrSet (timerType_t timerType, uint16_t period);



#endif