

#ifndef dahal_sser_h
#define dahal_sser_h

#include <ESP8266WiFi.h>

void DahalSserInit(void);
bool DahalSserAvailableData(void);
char DahalSserRead (void);




#endif