/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/
#include "daman_gps.h"
/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/
/** Software serial object */
SoftwareSerial ss(4,5);
/** Gps object, gives access to the gps functions and data handling */
TinyGPSPlus gps;
/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/
typedef enum
{
  WAIT_FOR_NMEA_LINE,
  MESSAGE_INIT,
  EVALUATE_HEADER,
  GGA_MESSAGE,
  RMC_MESSAGE,
  OTHER_NMEA_MESSAGE
}nmeaMessage_t;

typedef enum
{
  GGA,
  RMC,
  OTHERS
}headerName_t;


const char *gga = "GGA";
const char *rmc = "RMC";

/* GPS parser data */
nmeaMessage_t nmeaMessage = WAIT_FOR_NMEA_LINE;
char nmeaPacket[75];

void GpsNmeaParser (char character, packet_t *packet_gps);
void GpsNmeaBufferErase (void);
headerName_t GpsNmeaHeaderNameCheck (void);

void GpsNmeaParser (char character, packet_t *packet_gps)
{
  static int nmeaIndex;
  static int headerCounter;
  switch(nmeaMessage)
  {
    case WAIT_FOR_NMEA_LINE:
      if(character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
      }
      break;

    case MESSAGE_INIT:
      GpsNmeaBufferErase();
      nmeaIndex=0;
      nmeaPacket[nmeaIndex]=character;
      nmeaMessage = EVALUATE_HEADER;
      break;

    case EVALUATE_HEADER:
      nmeaIndex++;
      nmeaPacket[nmeaIndex]=character;
      headerCounter++;
      if(headerCounter>=5)
      {
        headerCounter = 0;
        switch (GpsNmeaHeaderNameCheck())
        {
          case GGA:
          nmeaMessage = GGA_MESSAGE;
          break;
          case RMC:
          nmeaMessage = RMC_MESSAGE;
          break;
          case OTHERS:
          nmeaMessage = OTHER_NMEA_MESSAGE;
        }
      }
      break;

    case GGA_MESSAGE:
      if (character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
        memcpy(packet_gps->ggaFrame, nmeaPacket, sizeof(nmeaPacket));
        //Serial.println(packet_gps->ggaFrame);
      }
      else
      {
        nmeaIndex++;
        nmeaPacket[nmeaIndex]=character;
      }
      break;

    case RMC_MESSAGE:
      if (character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
        memcpy(packet_gps->rmcFrame, nmeaPacket, sizeof(nmeaPacket));
        //Serial.println(packet_gps->rmcFrame);
      }
      else
      {
        nmeaIndex++;
        nmeaPacket[nmeaIndex]=character;
      }
      break;

    case OTHER_NMEA_MESSAGE:
      if (character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
      }
      else
      {
        nmeaIndex++;
        nmeaPacket[nmeaIndex]=character;
      }
      break;

    default:
      nmeaMessage = WAIT_FOR_NMEA_LINE;
      break;
  }
}

void GpsNmeaBufferErase (void)
{
  int nmeaIndex;
  for (nmeaIndex=0; nmeaIndex<75; nmeaIndex++)
  {
    nmeaPacket[nmeaIndex]=0;
  }
}

headerName_t GpsNmeaHeaderNameCheck (void)
{
  headerName_t headerName;
  bool result = true;
  int nmeaIndex;
  for (nmeaIndex=0; nmeaIndex<3; nmeaIndex++)
  {
    result &= (nmeaPacket[nmeaIndex+3] == gga[nmeaIndex]);
  }
  if (result==true)
  {
    headerName = GGA;
  }
  else
  {
    result=true;
    for (nmeaIndex=0; nmeaIndex<3; nmeaIndex++)
    {
      result &= (nmeaPacket[nmeaIndex+3] == rmc[nmeaIndex]);
    }
    if (result==true)
    {
      headerName = RMC;
    }
    else
    {
      headerName = OTHERS;
    }
    
  }
  return headerName;
}

void GpsHandle (packet_t *packet)
{
  while (ss.available() > 0)
  {
    GpsNmeaParser(ss.read(), packet);
  }
}

void GpsInit (void)
{
    ss.begin(9600);
}
