#ifndef da_main_h
#define da_main_h

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/
typedef enum
{
    HEARTBEAT,
    WIFI,
    UART
}events_t;
/*****************************************************************/
/*                  Public Function Prototypes                   */
/*****************************************************************/
bool GetEventFromQueue (events_t *event);
void PutEventInQueue (events_t event);
void DataAcquirerInit (void);
void Polling (void);
void PeriodicEvent (void);
void WifiEvent (void);
void UartEvent (void);



#endif