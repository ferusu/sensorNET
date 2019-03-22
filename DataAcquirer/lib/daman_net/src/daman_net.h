
#ifndef daman_net_h
#define daman_net_h
 
#include <ESP8266WiFi.h>

typedef enum
{
    CONNECT_WIFI = 0,
    ASK_FOR_ID_PORT,
    CONNECT_TO_UDP,
    UPDATE_PARAMETERS,
    CHECK_CONNECTION
}netOrders_t;

typedef enum
{
    SENSORS_DATA = 0,
    DATA_ACQUIRER_STATE,
    OUTGOING_ACK
}typeOfData_t;

typedef enum
{
    COMMAND = 0x01,
    CONNECT = 0x55,
    INCOMING_ACK
}typeOfIncomingData_t;

typedef enum
{
    START = 0,
    WAIT,
    SUCCESS,
    TIMEOUT
}operationResult_t;

typedef struct 
{
    bool activateGps;
    bool activateImu;
    bool activateImuFilter;
    bool command3;
    bool command4;
    bool command5;
    bool command6;
    bool command7;
    uint8_t samplingFrecquency;
}command_t;

typedef struct 
{
    char id;
    uint16_t port;
}idPort_t;

void DamanNetDiggestPacket (void);

/* Interface with main state machine */
operationResult_t DamanNetNetworkInterface (netOrders_t order);


operationResult_t DamanNetSend (typeOfData_t typeOfData, char *data, uint8_t dataLength);

void DamanNetInit (command_t *commandWord, idPort_t *idPortWord);

#endif