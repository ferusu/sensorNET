#include "daapp_stat.h"
#include "global_types.h"
#include "daman_wifi.h"
#include "daman_imu.h"

typedef enum
{
    RUNNING,
    SEND_ANSWER_COMMAND,
    CALIBRATE
}mainStates_t;

static mainStates_t mainState;

void StatPeriodicExecution (orderPacket_t *orderPacket, packet_t *packet)
{
    orderPacket_t lastOrderPacket;
    switch(mainState)
    {
        case RUNNING:
            SendBufferUdp (packet);
        break;
        case SEND_ANSWER_COMMAND:
            if((lastOrderPacket.imuAccelConfig != orderPacket->imuAccelConfig)||(lastOrderPacket.imuGyroConfig != orderPacket->imuGyroConfig))
            {
                mainState = CALIBRATE;
            }
            orderPacket->wifiStrenght = GetSignalStrenght ();
            SendCommandAnswer (orderPacket);
            mainState = (mainState == SEND_ANSWER_COMMAND)? RUNNING:mainState; // If a calibration is required, we keep the state as it was set
            //Otherwise we move forward to running again
        break;
        case CALIBRATE:
            ImuConfig (orderPacket->imuAccelConfig, orderPacket->imuGyroConfig);
            mainState = RUNNING;
        break;
        default:

        break;
    }
}

void StatCommandReceived (void)
{
    mainState = SEND_ANSWER_COMMAND;
}