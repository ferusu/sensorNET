#include <dahal.h>
#include <dahal_wifi.h>
#include <dahal_sser.h>
#include <dahal_i2c.h>
#include <dahal_tmr.h>
#include <dahal_evt.h>

DahalWifi DahalWifi_dahal();

void DahalInit (void)
{
    DahalSserInit();
    DahalI2cInit();
    DahalTmrInit ();
    DahalTmrSet(HEARTBEAT_TIMER, 100);
    DahalTmrSet(MAIN_STATE_MACHINE_TIMER, 1000);
}