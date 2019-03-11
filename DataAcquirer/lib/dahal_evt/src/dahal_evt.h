

#ifndef dahal_evt_h
#define dahal_evt_h

#include <ESP8266WiFi.h>

typedef enum
{
    SUCCESS = 0,
    ERROR = 1,
    NOT_KNOWN_EVENT_TYPE = 2,
    EVENT_QUEUE_OVERLOAD = 3,
    NOT_EVENT = 4,
    NOT_KNOWN_ORDER
}eventHandlingResult_t;

typedef enum
{
    DAHAL_EVENT_TYPE_NOT_EVENT = 0,
    DAHAL_EVENT_TYPE_WIFI,
    DAHAL_EVENT_TYPE_SOFTWARE_SERIAL,
    DAHAL_EVENT_TYPE_TIMER
}eventType_t;

typedef enum
{
    MAIN_STATE_MACHINE_TIMER = 0,
    HEARTBEAT_TIMER
}timerType_t;


typedef struct
{
    timerType_t timerType;
    uint32_t timestamp;
}timer_t;


typedef struct
{
    eventType_t enventType;
    timer_t timer;
}event_t;

class DahalEvt
{
  public:
    DahalEvt();
    bool eventGet(event_t *eventOutput);
  private:
    void softwareSerialPolling (void);
    void wifiPolling (void);
    event_t event;
};

eventHandlingResult_t EventQueuePut (event_t eventInput);

#endif