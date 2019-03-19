/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 13/03/2019                                           */
/*   Module: daapp_stat  File: daapp_stat.cpp                   */
/*   Brief:                                                     */
/****************************************************************/


/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/

#include <daapp_stat.h>
#include <daman_net.h>
#include <daman_gps.h>
#include <daman_imu.h>
#include <dahal_wifi.h>

/******************************************************************/
/*                     Definition of objects                      */
/******************************************************************/



/******************************************************************/
/*            Definition of local symbolic constants              */
/******************************************************************/



/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/

typedef enum
{
    BOOT = 0,
    CONNECT,
    RUNNING,
    TURN_OFF
}mainStates_t;

typedef enum
{
    IDLE = 0,
    ACTIVE_SENSORS,
    SEND_DATA,
    SEND_STATE,
    EDIT_PARAMETERS
}runningStates_t;

typedef enum
{
    CONNECT_WIFI_STAT = 0,
    GET_ID_STAT,
    CONNECT_UDP_STAT,
    SUCCESS_CONNECTION_STAT
}connectStates_t;

typedef struct
{
    /* Command dependant */
    bool imuActive;
    bool gpsActive;
    bool imuFilterActive;
    uint8_t samplingFrecquency; 
    /* DataAcquirer dependant */
    uint8_t batteryState;
    uint8_t trackedSatellites;
    bool activeLocation;
    bool activeGpsTime;
    bool batteryCharging;
}systemState_t;

/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/



/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

/* General module variables */
static bool daappStatInitialized = false;
/* State machine variables */
static mainStates_t mainState = BOOT;
static runningStates_t runningState = IDLE;
static bool firstTimeStateMSM = true;
static bool firstTimeStateRSM = true;
static bool runningAuthorization = false;
static bool connected = false;
static bool transmissionEnded = false;
static bool transmissionTimeout = false;
/* Command and state variables */
static command_t commandStored;
static systemState_t systemState;
static char systemStatePackage[12];
static idPort_t idPortStat;
/* Data vector */
static char dataPackage[40];
static gpsData_t *gpsData;
static imuData_t *imuData;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

/*Main state machine states*/
void BootState (void);
void ConnectState (void);
void RunningState (void);
void TurnOffState (void);
/*Main state machine guards*/
mainStates_t BootStateGuards (void);
mainStates_t ConnectStateGuards (void);
mainStates_t RunningStateGuards (void);
mainStates_t TurnOffStateGuards (void);
/*Main state machine function*/
void DaappStatMainStateMachine (void);
/*Running state machine states*/
void IdleState (void);
void ActiveSensorsState (void);
void SendDataState (void);
void SendStateState (void);
void EditParameters (void);
/*Running state machine guards*/
runningStates_t IdleStateGuards (void);
runningStates_t ActiveSensorsStateGuards (void);
runningStates_t SendDataStateGuards (void);
runningStates_t SendStateStateGuards (void);
runningStates_t EditParametersGuards (void);
/*Running state machine function*/
void DaappStatRunningStateMachine (void);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

/*Main state machine states*/
void BootState (void)
{
    if(firstTimeStateMSM)
    {
        firstTimeStateMSM = false;
    }
}

void ConnectState (void)
{
    static connectStates_t internConnectState = CONNECT_WIFI_STAT;
    if(firstTimeStateMSM)
    {
        firstTimeStateMSM = false;
        internConnectState = CONNECT_WIFI_STAT;
    }
    switch (internConnectState)
    {
        case CONNECT_WIFI_STAT:
            internConnectState = (DamanNetNetworkInterface(CONNECT_WIFI)==SUCCESS)?GET_ID_STAT:CONNECT_WIFI_STAT;
            break;
        case GET_ID_STAT:
            internConnectState = (DamanNetNetworkInterface(ASK_FOR_ID_PORT)==SUCCESS)?CONNECT_UDP_STAT:GET_ID_STAT;
            break;
        case CONNECT_UDP_STAT:
            internConnectState = (DamanNetNetworkInterface(CONNECT_TO_UDP)==SUCCESS)?SUCCESS_CONNECTION_STAT:CONNECT_UDP_STAT;
            break;
        case SUCCESS_CONNECTION_STAT:
            internConnectState = CONNECT_WIFI_STAT;
            connected = true;
            break;
        default:
            break;
    }
}

void RunningState (void)
{
    if(firstTimeStateMSM)
    {
        firstTimeStateMSM = false;
    }
    runningAuthorization = true;
    if(mainState != RUNNING)
    {
        runningAuthorization = false;
    }
}

void TurnOffState (void)
{
    if(firstTimeStateMSM)
    {
        firstTimeStateMSM = false;
    }
    /*Interface with battery manager*/
}

/*Main state machine guards*/
mainStates_t BootStateGuards (void)
{
    mainStates_t returnAnswer = BOOT;
    returnAnswer = (daappStatInitialized)?CONNECT:BOOT;
    /*Interface with battery manager to change to turn-off state*/
    return returnAnswer;
}

mainStates_t ConnectStateGuards (void)
{
    mainStates_t returnAnswer = CONNECT;
    returnAnswer = (connected)?RUNNING:CONNECT;
    /*Interface with battery manager to change to turn-off state*/
    return returnAnswer;
}

mainStates_t RunningStateGuards (void)
{
    mainStates_t returnAnswer = RUNNING;
    returnAnswer = (DamanNetNetworkInterface(CHECK_CONNECTION)==SUCCESS)?RUNNING:CONNECT;
    /*Interface with battery manager to change to turn-off state*/
    return returnAnswer;
}

mainStates_t TurnOffStateGuards (void)
{
    mainStates_t returnAnswer = TURN_OFF;
    /*Interface with battery manager to change to turn-off state*/
    return returnAnswer;
}

/*Main state machine function*/
void DaappStatMainStateMachine (void)
{
    switch (mainState)
    {
        case BOOT:
            mainState = BootStateGuards();
            BootState();
            firstTimeStateMSM = (mainState != BOOT)?true:firstTimeStateMSM;
            break;
        case CONNECT:
            mainState = ConnectStateGuards();
            ConnectState();
            firstTimeStateMSM = (mainState != CONNECT)?true:firstTimeStateMSM;
            break;
        case RUNNING:
            mainState = RunningStateGuards();
            RunningState();
            firstTimeStateMSM = (mainState != RUNNING)?true:firstTimeStateMSM;
            break;
        case TURN_OFF:
            mainState = BootStateGuards();
            TurnOffState();
            firstTimeStateMSM = (mainState != TURN_OFF)?true:firstTimeStateMSM;
            break;
        default:

            break;
    }
}

/*Running state machine states*/
void IdleState (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
        systemState.activeGpsTime = true;
        systemState.activeLocation = true;
    }
}

void ActiveSensorsState (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
    }
}

void SendDataState (void)
{
    uint8_t packetSize = 0;
    operationResult_t sendingResult;
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
        DamanImuGet(imuData);
        dataPackage[0]=idPortStat.id;
        memcpy((dataPackage+1),imuData,sizeof(imuData_t));
        memcpy((dataPackage+sizeof(imuData_t)+1),gpsData,sizeof(gpsData_t));
        packetSize = (uint8_t)(sizeof(imuData_t)+sizeof(gpsData_t));
    }
    sendingResult = DamanNetSend (SENSORS_DATA, dataPackage, packetSize);
    transmissionEnded = (sendingResult==SUCCESS)?true:false;
    transmissionTimeout = (sendingResult == TIMEOUT)?true:false;
}

void SendStateState (void)
{
    uint8_t packetSize = 0;
    operationResult_t sendingResult;
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
        systemStatePackage[0]=idPortStat.id;
        systemStatePackage[1]=(char)systemState.imuActive;
        systemStatePackage[2]=(char)systemState.gpsActive;
        systemStatePackage[3]=(char)systemState.imuFilterActive;
        systemStatePackage[4]=(char)systemState.samplingFrecquency; 
        systemStatePackage[5]=(char)systemState.batteryState;
        systemStatePackage[6]=(char)systemState.trackedSatellites;
        systemStatePackage[7]=(char)systemState.activeLocation;
        systemStatePackage[8]=(char)systemState.activeGpsTime;
        systemStatePackage[9]=(char)systemState.batteryCharging;
        packetSize = sizeof(systemStatePackage);
    }
    sendingResult = DamanNetSend (DATA_ACQUIRER_STATE, systemStatePackage, packetSize);
    transmissionEnded = (sendingResult==SUCCESS)?true:false;
    transmissionTimeout = (sendingResult == TIMEOUT)?true:false;
}

void EditParameters (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
    }
    systemState.gpsActive = commandStored.activateGps;
    systemState.imuActive = commandStored.activateImu;
    systemState.imuFilterActive = commandStored.activateImuFilter;
    systemState.samplingFrecquency = commandStored.samplingFrecquency;
}

/*Running state machine guards*/
runningStates_t IdleStateGuards (void)
{
    runningStates_t returnAnswer = IDLE;
    returnAnswer = (DamanNetNetworkInterface(UPDATE_PARAMETERS)==SUCCESS)?EDIT_PARAMETERS:IDLE;
    return returnAnswer;
}

runningStates_t ActiveSensorsStateGuards (void)
{
    static uint8_t prescaler = 0;
    runningStates_t returnAnswer = ACTIVE_SENSORS;
    if(prescaler>=systemState.samplingFrecquency)
    {
        prescaler = 0;
        returnAnswer = SEND_DATA;
    }
    else
    {
        prescaler++;
    }
    returnAnswer = (DamanNetNetworkInterface(UPDATE_PARAMETERS)==SUCCESS)?EDIT_PARAMETERS:returnAnswer;
    return returnAnswer;
}

runningStates_t SendDataStateGuards (void)
{
    runningStates_t returnAnswer = SEND_DATA;
    returnAnswer = (transmissionEnded||transmissionTimeout)?ACTIVE_SENSORS:SEND_DATA;
    transmissionEnded = false;
    transmissionTimeout = false;
    return returnAnswer;
}

runningStates_t SendStateStateGuards (void)
{
    runningStates_t returnAnswer = SEND_STATE;
    if(transmissionEnded||transmissionTimeout)
    {
        returnAnswer = (systemState.gpsActive||systemState.imuActive)?ACTIVE_SENSORS:IDLE;
        transmissionEnded = false;
        transmissionTimeout = false;
    }
    return returnAnswer;
}

runningStates_t EditParametersGuards (void)
{
    runningStates_t returnAnswer = EDIT_PARAMETERS;
    returnAnswer = SEND_STATE; 
    return returnAnswer;
}

/*Running state machine function*/
void DaappStatRunningStateMachine (void)
{
    if (runningAuthorization)
    {
        switch(runningState)
        {
            case IDLE:
                runningState = IdleStateGuards ();
                IdleState ();
                firstTimeStateRSM = (runningState != IDLE)?true:firstTimeStateRSM;
                break;
            case ACTIVE_SENSORS:
                runningState = ActiveSensorsStateGuards ();
                ActiveSensorsState ();
                firstTimeStateRSM = (runningState != ACTIVE_SENSORS)?true:firstTimeStateRSM;
                break;
            case SEND_DATA:
                runningState = SendDataStateGuards ();
                SendDataState ();
                firstTimeStateRSM = (runningState != SEND_DATA)?true:firstTimeStateRSM;
                break;
            case SEND_STATE:
                runningState = SendStateStateGuards ();
                SendStateState ();
                firstTimeStateRSM = (runningState != SEND_STATE)?true:firstTimeStateRSM;
                break;
            case EDIT_PARAMETERS:
                runningState = EditParametersGuards ();
                EditParameters ();
                firstTimeStateRSM = (runningState != EDIT_PARAMETERS)?true:firstTimeStateRSM;
                break;
            default:

                break;
        }

    }
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void DaappStatFirstTimeBase (void)
{
    DaappStatMainStateMachine();
}

void DaappStatSecondTimeBase (gpsInterface_t gpsState)
{
    if (gpsState == NEW_TIME)
    {
        systemState.activeGpsTime = true;
    }
    if (gpsState == NEW_DATA_AND_TIME)
    {
        systemState.activeLocation = true;
    }
    
    DaappStatRunningStateMachine();
}

void DaappStatInit (gpsData_t *gpsDataInput)
{
    daappStatInitialized = true;
    DamanNetInit(&commandStored, &idPortStat);
    DamanImuInit();
    gpsData = gpsDataInput;
}