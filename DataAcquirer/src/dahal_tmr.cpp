#include <ESP8266WiFi.h>
#include "dahal_tmr.h"
#include "da_main.h"
#include "global_types.h"

os_timer_t softwareTimer;

static uint32_t heartbeatTimestamp = 0;
static bool heartbeatTick = false;
static int heartbeatPeriod = 100;

void timerCallback(void *pArg)
{
    heartbeatTimestamp++;
    heartbeatTick = true;
    PutEventInQueue (HEARTBEAT);
}

void TimerInit (void)
{
    os_timer_setfn(&softwareTimer, timerCallback, NULL);
    os_timer_arm(&softwareTimer, (int)heartbeatPeriod, true);
}