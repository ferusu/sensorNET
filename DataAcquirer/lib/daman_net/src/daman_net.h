
#ifndef daman_net_h
#define daman_net_h
 
#include <ESP8266WiFi.h>

typedef enum
{
    CONNECT_WIFI = 0,
    ASK_FOR_ID_PORT,
    CONNECT_TO_UDP,
    UPDATE_PARAMETERS
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
    /* data */
};


void DamanNetDiggestPacket (void);

/* Interface with main state machine */
operationResult_t DamanNetNetworkInterface (netOrders_t order);


operationResult_t DamanNetSend (typeOfData_t typeOfData, uint8_t *data, uint8_t dataLength);

#endif