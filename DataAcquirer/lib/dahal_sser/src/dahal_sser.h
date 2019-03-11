

#ifndef dahal_sser_h
#define dahal_sser_h

#include <ESP8266WiFi.h>

class DahalSser
{
  public:
    DahalSser();
    bool availableData();
    char read();
  private:
    bool _dahalSserInitialized;
};

void DahalSserInit (void);

#endif