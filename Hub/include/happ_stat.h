#ifndef happ_stat_h
#define happ_stat_h

typedef enum
{
    RUNNING,
    SEND_COMMAND_DATA_COLLECTER,
    SEND_COMMAND_DATA_ACQUIRER
}mainStates_t;

void HStatPeriodicExecution (orderPacket_t *orderPacket, packet_t *packet);
void HStatCommandChange (mainStates_t newState);


#endif