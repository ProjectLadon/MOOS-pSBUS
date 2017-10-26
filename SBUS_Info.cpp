/****************************************************************/
/*   NAME:                                              */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: SBUS_Info.cpp                               */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "SBUS_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pSBUS application is used for connecting an SBUS R/C      ");
  blk("receiver to a MOOS community. It pulls all eighteen channels    ");
  blk("(16 proportional and 2 binary) along with a failsafe binary     ");
  blk("channel that is true if no signal is received from the          ");
  blk("transmitter.                                                    ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pSBUS file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pSBUS with the given process name         ");
  blk("      rather than pSBUS.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pSBUS.        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pSBUS Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pSBUS                              ");
  blk("{                                                               ");
  blk("  AppTick   = 10                                                ");
  blk("  CommsTick = 4                                                 ");
  blk("  SBUS_PORT = /dev/ttyO4                                        ");
  blk("  SBUS_MAXVALUE = 2000                                          ");
  blk("  SBUS_MINVALUE = 1000                                          ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pSBUS INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  pSBUS does not to subscribe to any variables                  ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("                                                                ");
  blk("  SBUS_json -- a STRING that containing a JSON object with raw  ");
  blk("               raw and scaled channels.                         ");
  blk("  SBUS_Channels -- a STRING containing a JSON array with the    ");
  blk("                   proportional values of the first sixteen     ");
  blk("                   channels in microseconds                     ");
  blk("  SBUS_Scaled_Channels -- a STRING containing a JSON array with ");
  blk("                          the scaled values of the first sixteen");
  blk("                          channels in the range -1.0f to 1.0f.  ");
  blk("  SBUS_Ch17 -- a BINARY containing the received value of channel");
  blk("               17                                               ");
  blk("  SBUS_Ch18 -- a BINARY containing the received value of channel");
  blk("               18                                               ");
  blk("  SBUS_Failsafe -- a BINARY that is true if the receiver is in  ");
  blk("                   failsafe or missing and false otherwise.     ");
  blk("  SBUS_GoodFrames -- a DOUBLE containing the number of good     ");
  blk("                     frames received.                           ");
  blk("  SBUS_BadFrames -- a DOUBLE containing the number of bad frames");
  blk("                    received.                                   ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pSBUS", "gpl");
  exit(0);
}

