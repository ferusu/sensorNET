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
/* Command and state variables */
static command_t commandStored;
static systemState_t systemState;

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
    }
}

void ActiveSensorsState (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
    }
    systemState.gpsActive;
    systemState.imuActive;
}

void SendDataState (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
    }
}

void SendStateState (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
    }
}

void EditParameters (void)
{
    if(firstTimeStateRSM)
    {
        firstTimeStateRSM = false;
    }
}

/*Running state machine guards*/
runningStates_t IdleStateGuards (void)
{
    runningStates_t returnAnswer = IDLE;

    return returnAnswer;
}

runningStates_t ActiveSensorsStateGuards (void)
{
    runningStates_t returnAnswer = ACTIVE_SENSORS;

    return returnAnswer;
}

runningStates_t SendDataStateGuards (void)
{
    runningStates_t returnAnswer = SEND_DATA;

    return returnAnswer;
}

runningStates_t SendStateStateGuards (void)
{
    runningStates_t returnAnswer = SEND_STATE;

    return returnAnswer;
}

runningStates_t EditParametersGuards (void)
{
    runningStates_t returnAnswer = EDIT_PARAMETERS;

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

void DaappStatSecondTimeBase (gpsData_t *gpsDataInput, gpsInterface_t gpsState)
{
    
}

void DaappStatInit (void)
{
    daappStatInitialized = true;
    DamanNetInit(&commandStored);
}