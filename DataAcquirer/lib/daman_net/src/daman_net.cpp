/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 13/03/2019                                           */
/*   Module: daman_net   File: daman_net.cpp                    */
/*   Brief:                                                     */
/****************************************************************/


/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <daman_net.h>
#include <dahal_wifi.h>

/******************************************************************/
/*                     Definition of objects                      */
/******************************************************************/



/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/

#define UDP_TIMEOUT     (0x14)
#define CONNECT_TIMEOUT (0x100)
#define UDP_PORT_MAX    (1100)
#define UDP_PORT_MIN    (1000)

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/

typedef struct 
{
    char id;
    uint16_t port;
}idPort_t;

/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/

const idPort_t connectIdPort = {.id =0x55, .port = 0xFFFF};

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

static bool damanNetInitialized = false;
static idPort_t idPort;
static bool udpIdPortGranted = false;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

operationResult_t DamanNetWifiConnect (void);
operationResult_t DamanNetUdpConnect (void);
operationResult_t DamanNetAskForIdAndPort (void);
void DamanNetIncomingDataParse (typeOfIncomingData_t incomingData, uint8_t *incomingPacketToParse, uint8_t lengthOfPacket);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

operationResult_t DamanNetWifiConnect (void)
{
    static operationResult_t functionState = START;
    switch (functionState)
    {
        case START:
            DahalWifiInit();
            functionState = WAIT;
            break;
        case WAIT:
            functionState = (DahalWifiStatus() == WL_CONNECTED)?SUCCESS:WAIT;
            functionState = (DahalWifiStatus() == WL_CONNECT_FAILED)?TIMEOUT:functionState;
            break;
        case SUCCESS:
            /* Connection Success */
            functionState = START;
            break;
        case TIMEOUT:
            functionState = START;
            break;
    }
    return functionState;
}

operationResult_t DamanNetUdpConnect (void)
{
    static operationResult_t functionState = START;
    bool udpConnected = false;
    uint8_t timeout = 0;
    bool timeoutExceeded = false;
    while ((!udpConnected)&&(!timeoutExceeded))
    {
        udpConnected = DahalWifiUdpInit (idPort.port);
        timeout++;
        if (timeout > UDP_TIMEOUT)
        {
            functionState = TIMEOUT;
            timeoutExceeded = true;
        }
    }
    functionState = (udpConnected)?SUCCESS:functionState;
    return functionState;
}

operationResult_t DamanNetAskForIdAndPort (void)
{
    static operationResult_t functionState = START;
    static char connectIdPortTemp = connectIdPort.id;
    bool udpConnected = false;
    uint8_t timeout = 0;
    bool timeoutExceeded = false;
    switch (functionState)
    {
        case START:
            while ((!udpConnected)&&(!timeoutExceeded))
            {
                udpConnected = DahalWifiUdpInit (connectIdPort.port);
                timeout++;
                if (timeout > UDP_TIMEOUT)
                {
                    functionState = TIMEOUT;
                    timeoutExceeded = true;
                }
            }
            if (udpConnected)
            {
                DahalWifiSend(&connectIdPortTemp);
                functionState = WAIT;
                timeout = 0;
            }
            break;
        case WAIT:
            if(udpIdPortGranted)
            {
                functionState = SUCCESS;
            }
            else
            {
                timeout++;
                functionState = (timeout>CONNECT_TIMEOUT)?TIMEOUT:WAIT;
            }
            break;
        case SUCCESS:
            functionState = START;
            break;
        case TIMEOUT:
            functionState = START;
            break;
    }
    return functionState;
}



void DamanNetIncomingDataParse (typeOfIncomingData_t incomingData, char *incomingPacketToParse, int lengthOfPacket)
{
    switch (incomingData)
    {
        case COMMAND:
            /*Falta definir como será el comando para en función de eso parsearlo, castearlo, enmascararlo
            y hacer las operaciones que hagan falta para gestionar las ordenes provinientes de hub */
            break;
        case CONNECT: 
            idPort.id = *(incomingPacketToParse+1);
            idPort.port =(((uint16_t)(*(incomingPacketToParse+2)))<<8)|(*(incomingPacketToParse+3));
            udpIdPortGranted = ((idPort.port<UDP_PORT_MAX)&&(idPort.port>=UDP_PORT_MIN))?true:false;
            break;
        case INCOMING_ACK:
            /* Parsear el ack */
            break;
        default:
            /* Fallo de interfaz */
            break;
    }
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/


void DamanNetDiggestPacket (void)
{
    udpData_t incomingData;
    char *packet;
    uint8_t crc;
    /*Provisional*/
    DahalWifiRead(&incomingData);
    packet = incomingData.udpPacket;
    crc = DahalWifiCrc (packet, incomingData.packetSize);
    if (crc == *(packet+incomingData.packetSize-1))
    {
        DamanNetIncomingDataParse((typeOfIncomingData_t)*packet, packet, incomingData.packetSize);
    }
    else
    {
        /* Gestionar error */
    }
    

}

/* Interface with main state machine */
operationResult_t DamanNetNetworkInterface (netOrders_t order)
{
    operationResult_t returnAnswer = START;
    switch (order)
    {
        case CONNECT_WIFI:
            returnAnswer = DamanNetWifiConnect();
            break;
        case ASK_FOR_ID_PORT:
            returnAnswer = DamanNetAskForIdAndPort();
            break;
        case CONNECT_TO_UDP:
            returnAnswer = DamanNetUdpConnect();
            break;
        case UPDATE_PARAMETERS:
            /* Falta pensar esto */
            /* Si hay parámetros nuevos devuelve success */
            break;
        default:

            break;
    }
}


operationResult_t DamanNetSend (typeOfData_t typeOfData, uint8_t *data, uint8_t dataLength)
{
    /* Antes de enviar hay que calcular el crc y el operation result devolverá success cuando el 
    dato enviado reciba el acknowledge con el crc */
    /* De momento no vamos a garantizar la integridad del envío mediante un método ack aunque será
    el siguiente paso en el desarrollo de el net manager */
    switch (typeOfData)
    {
        case SENSORS_DATA:

            break;
        case DATA_ACQUIRER_STATE:

            break;
        case OUTGOING_ACK:

            break;
        default:

            break;
    }
}
