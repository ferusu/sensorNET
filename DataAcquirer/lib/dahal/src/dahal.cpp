#include <dahal.h>
#include <dahal_wifi.h>
#include <dahal_sser.h>
#include <dahal_i2c.h>
#include <dahal_tmr.h>
#include <dahal_evt.h>


void DahalInit (void)
{
    DahalSserInit();
    Serial.println("SoftwareSerial Initialized");
    DahalI2cInit();
    Serial.println("I2C Initialized");
    DahalTmrInit ();
    Serial.println("Tmr Initialized");
    DahalTmrSet(HEARTBEAT_TIMER, 1000);
    Serial.println("Heartbeat timer started");
    DahalTmrSet(MAIN_STATE_MACHINE_TIMER, 10000);
    Serial.println("Main state machine timer started");
}