
#ifndef daapp_stat_h
#define daapp_stat_h
 
#include <ESP8266WiFi.h>

void DaappStatFirstTimeBase (void);
void DaappStatSecondTimeBase (gpsInterface_t gpsState);
void DaappStatInit (gpsData_t *gpsDataInput);

#endif