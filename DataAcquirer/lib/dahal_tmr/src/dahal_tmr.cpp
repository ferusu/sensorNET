
#include <dahal_tmr.h>

os_timer_t softwareTimer;
static bool timerInit = false;
static uint16_t prescaler;

void timerCallback(void *pArg)
{


}

void DahalTmrSet (timerType_t timerType, uint16_t period)
{
    static uint16_t mainStateMachinePeriod = 0;
    static uint16_t heartbeatPeriod = 0;
    if(timerInit)
    {
        switch(timerType)
        {
            case MAIN_STATE_MACHINE_TIMER:
                if ((period>heartbeatPeriod)&&(heartbeatPeriod>0))
                {
                    prescaler = period/heartbeatPeriod;
                    mainStateMachinePeriod = heartbeatPeriod*prescaler;
                    Serial.println(mainStateMachinePeriod);
                }
                break;
            case HEARTBEAT_TIMER:
                heartbeatPeriod = period;
                Serial.println(heartbeatPeriod);
                break;
        }
        os_timer_arm(&softwareTimer, (int)heartbeatPeriod, true);
    }
}

void DahalTmrInit (void)
{
    os_timer_setfn(&softwareTimer, timerCallback, NULL);
    timerInit = true;
}