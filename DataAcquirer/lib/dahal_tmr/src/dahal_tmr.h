#ifndef dahal_tmr_h
#define dahal_tmr_h

#include <ESP8266WiFi.h>
#include <dahal_evt.h>

void DahalTmrInit (void);
void DahalTmrSet (timerType_t timerType, uint16_t period);



#endif