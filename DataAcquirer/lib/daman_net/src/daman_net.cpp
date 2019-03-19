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
#include <daman_imu.h>
#include <daman_gps.h>

/******************************************************************/
/*                     Definition of objects                      */
/******************************************************************/



/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/

#define UDP_TIMEOUT     (0x14)
#define ACK_TIMEOUT     (0x14)
#define CONNECT_TIMEOUT (0x100)
#define UDP_PORT_MAX    (1100)
#define UDP_PORT_MIN    (1000)
#define LESS_SIGNIFICANT_BYTE_MASK      (0x00FF)

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
const uint16_t positiveMasks[16] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

static bool damanNetInitialized = false;
static idPort_t idPort;
static bool udpIdPortGranted = false;
static char currentAck = 0x00;
static uint16_t currentCommand = 0;
static bool newCommand = false;
static command_t *commandPointer;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

operationResult_t DamanNetWifiConnect (void);
operationResult_t DamanNetUdpConnect (void);
operationResult_t DamanNetAskForIdAndPort (void);
void DamanNetParseCommandData (void);
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

void DamanNetParseCommandData (uint16_t comingCommand)
{
    uint8_t bitIndex = 8;
    commandPointer->activateGps = comingCommand & positiveMasks[bitIndex++];
    commandPointer->activateImu = comingCommand & positiveMasks[bitIndex++];
    commandPointer->activateImuFilter = comingCommand & positiveMasks[bitIndex++];
    commandPointer->command3 = comingCommand & positiveMasks[bitIndex++];
    commandPointer->command4 = comingCommand & positiveMasks[bitIndex++];
    commandPointer->command5 = comingCommand & positiveMasks[bitIndex++];
    commandPointer->command6 = comingCommand & positiveMasks[bitIndex++];
    commandPointer->command7 = comingCommand & positiveMasks[bitIndex++];
    commandPointer->samplingFrecquency = (uint8_t)comingCommand;
}

void DamanNetIncomingDataParse (typeOfIncomingData_t incomingData, char *incomingPacketToParse, int lengthOfPacket)
{
    uint16_t comingCommand;
    switch (incomingData)
    {
        case COMMAND:
            comingCommand = (((uint16_t)(*(incomingPacketToParse+1)))<<8)|(*(incomingPacketToParse+2));
            if (comingCommand != currentCommand)
            {
                newCommand = true;
                DamanNetParseCommandData(comingCommand);
                currentCommand = comingCommand;
            }
            break;
        case CONNECT: 
            idPort.id = *(incomingPacketToParse+1);
            idPort.port =(((uint16_t)(*(incomingPacketToParse+2)))<<8)|(*(incomingPacketToParse+3));
            udpIdPortGranted = ((idPort.port<UDP_PORT_MAX)&&(idPort.port>=UDP_PORT_MIN))?true:false;
            break;
        case INCOMING_ACK:
            /* Parsear el ack */
            currentAck = *(incomingPacketToParse+1);
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
    if (crc == *(packet+incomingData.packetSize-2)) //Menos dos que coincide con el dato del crc
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
    static operationResult_t returnAnswer = START;
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
            returnAnswer = (newCommand)?SUCCESS:WAIT;
            newCommand = false;
            break;
        case CHECK_CONNECTION:
            returnAnswer = (DahalWifiStatus() == WL_CONNECTED)?SUCCESS:TIMEOUT;
            break;
        default:

            break;
    }
}


operationResult_t DamanNetSend (typeOfData_t typeOfData, char *data, uint8_t dataLength)
{
    /* Antes de enviar hay que calcular el crc y el operation result devolverá success cuando el 
    dato enviado reciba el acknowledge con el crc */
    /* De momento no vamos a garantizar la integridad del envío mediante un método ack aunque será
    el siguiente paso en el desarrollo de el net manager */
    /* HAY QUE ADJUNTAR UN CERO AL FINAL DE CADA MENSAJE PARA DEFINIR LA LONGITUD DEL STRING QUE SE ENVÍA */
    static operationResult_t sendState = START;
    static uint8_t crc = 0;
    static uint8_t timeout = 0;
    switch (sendState)
    {
        case START:
            crc = DahalWifiCrc (data, dataLength+2); //Con el más dos evitamos que la función crc considere que el penúltimo dato es ya un crc y el último es '/0'
            *(data+dataLength) = (char)crc;
            *(data+dataLength+1)='\0'; //Close the string with the appropiate character 
            DahalWifiSend(data);
            sendState = WAIT;
            timeout = 0;
            break;
        case WAIT:
            if(currentAck == crc)
            {
                sendState = SUCCESS;
            }
            else
            {
                timeout++;
            }
            sendState = (timeout>ACK_TIMEOUT)?TIMEOUT:sendState;
            break;
        case SUCCESS:
        case TIMEOUT:
            sendState = START;
            timeout = 0;
            break;
        default:

            break;
    }
    currentAck;

}

void DamanNetInit (command_t *commandWord)
{
    commandPointer = commandWord;
}