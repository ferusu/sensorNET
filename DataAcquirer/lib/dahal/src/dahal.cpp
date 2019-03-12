#include <dahal.h>
#include <dahal_wifi.h>
#include <dahal_sser.h>
#include <dahal_i2c.h>
#include <dahal_tmr.h>
#include <dahal_evt.h>

DahalWifi DahalWifi_dahal();
DahalSser DahalSser_dahal();
DahalI2c  DahalI2c_dahal();

void DahalInit (void)
{
    DahalTmrInit ();
    DahalTmrSet(HEARTBEAT_TIMER, 100);
    DahalTmrSet(MAIN_STATE_MACHINE_TIMER, 1000);
}