#include <dahal_sser.h>
#include <SoftwareSerial.h>     // The serial connection to the GPS module

SoftwareSerial ss(4,5);
bool dahalSserInitialized = false;

class DahalSser
{
  public:
    DahalSser();
    bool availableData();
    char read();
  private:
    bool _dahalSserInitialized;
};

DahalSser::DahalSser()
{
    _dahalSserInitialized = dahalSserInitialized;
}

bool DahalSser::availableData()
{
    bool returnValue = false;
    if (_dahalSserInitialized)
    {
        returnValue = ss.available();
    }
    return returnValue;
}

char DahalSser::read()
{
    char returnValue = 0;
    if (_dahalSserInitialized)
    {
        returnValue = ss.read();
    }
    return returnValue;
}

void DahalSserInit (void)
{
    ss.begin(9600);
    dahalSserInitialized = true;
}