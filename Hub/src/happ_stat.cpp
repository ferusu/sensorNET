#include "happ_stat.h"
#include "h_global_types.h"
#include "hman_wifi.h"

static mainStates_t mainState;

void HStatPeriodicExecution (orderPacket_t *orderPacket, packet_t *packet)
{
    static orderPacket_t lastOrderPacket;
    static uint32_t timestamp;
    switch(mainState)
    {
        case RUNNING:
            timestamp++;
            if(timestamp >= 9)
            {
                timestamp = 0;
                mainState = SEND_COMMAND_DATA_ACQUIRER;
            }
        break;
        case SEND_COMMAND_DATA_COLLECTER:
            if((lastOrderPacket.imuAccelConfig != orderPacket->imuAccelConfig)||(lastOrderPacket.imuGyroConfig != orderPacket->imuGyroConfig))
            {
                mainState = SEND_COMMAND_DATA_ACQUIRER;
                memcpy(&lastOrderPacket,orderPacket,sizeof(orderPacket_t));
            }
            mainState = (mainState == SEND_COMMAND_DATA_ACQUIRER)? RUNNING:mainState; // If a calibration is required, we keep the state as it was set
            //Otherwise we move forward to running again
        break;
        case SEND_COMMAND_DATA_ACQUIRER:
            HTransmitCommand (orderPacket);
            mainState = RUNNING;
        break;
        default:

        break;
    }
}

void HStatCommandChange (mainStates_t newState)
{
    mainState = newState;
}