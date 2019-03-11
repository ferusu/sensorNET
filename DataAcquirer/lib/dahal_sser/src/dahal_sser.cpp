#include <dahal_sser.h>
#include <SoftwareSerial.h>     // The serial connection to the GPS module

const int receivePin = 4;
const int transmitPin = 5;

SoftwareSerial ss(receivePin,transmitPin);
bool dahalSserInitialized = false;


void DahalSserInit (void);
void DahalSserInit (void)
{
    ss.begin(9600);
    dahalSserInitialized = true;
}

DahalSser::DahalSser()
{
    if(!dahalSserInitialized)
    {
        DahalSserInit();
    }
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