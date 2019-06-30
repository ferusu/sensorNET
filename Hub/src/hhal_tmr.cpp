#include <ESP8266WiFi.h>
#include "hhal_tmr.h"
#include "h_main.h"
#include "h_global_types.h"

os_timer_t softwareTimer;

static uint32_t heartbeatTimestamp = 0;
static bool heartbeatTick = false;
static int heartbeatPeriod = 100;

void HtimerCallback(void *pArg)
{
    heartbeatTimestamp++;
    heartbeatTick = true;
    PutEventInQueue (HEARTBEAT);
}

void HTimerInit (void)
{
    os_timer_setfn(&softwareTimer, HtimerCallback, NULL);
    os_timer_arm(&softwareTimer, (int)heartbeatPeriod, true);
}