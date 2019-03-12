#include <dahal_sser.h>
#include <SoftwareSerial.h>     // The serial connection to the GPS module

const int receivePin = 4;
const int transmitPin = 5;

SoftwareSerial ss(receivePin,transmitPin);
bool dahalSserInitialized = false;


void DahalSserInit (void)
{
    if(!dahalSserInitialized)
    {
        ss.begin(9600);
        dahalSserInitialized = true;
    }
}

bool DahalSserAvailableData(void)
{
    bool returnValue = false;
    if (dahalSserInitialized)
    {
        returnValue = ss.available();
    }
    return returnValue;
}

char DahalSserRead (void)
{
    char returnValue = 0;
    if (dahalSserInitialized)
    {
        returnValue = ss.read();
    }
    return returnValue;
}