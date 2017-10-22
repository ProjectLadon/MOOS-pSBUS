/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SBUS.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "SBUS.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <inttypes.h>

using namespace std;

//---------------------------------------------------------
// Constructor

SBUS::SBUS()
{
}

//---------------------------------------------------------
// Destructor

SBUS::~SBUS()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SBUS::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SBUS::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SBUS::Iterate()
{
  AppCastingMOOSApp::Iterate();
  char buf[SBUS_BUF_LEN]
  ssize_t bytesRead = read(devFD, buf, SBUS_BUF_LEN);

  if (bytesRead > 0) {
    for (int i = 0; i < bytesRead; i++) m_buf.push_back(buf[i]); // we have to do this in order to copy /0 correctly
    cerr << "Read " << to_string(bytesRead) << " bytes into m_buf: [" << m_buf << "]";
    memset(buf, 0, SBUS_BUF_LEN);
  }

  if (buf.size() >= SBUS_BUF_LEN) {
    if ((buf[0] != SBUS_STARTBYTE) || (buf[(SBUS_BUF_LEN - 1)] != SBUS_ENDBYTE)) {
      cerr << "Received invalid frame: [" << buf << "]";
      buf.clear();
      m_errorFrames++;
      m_valid = false;
      return false;
    } else {
      m_goodFrames++;
      m_valid = true;
      setLastInputTime();

      m_raw_channels[0]  = ((buf[1]    |buf[2]<<8)         & 0x07FF);
      m_raw_channels[1]  = ((buf[2]>>3 |buf[3]<<5)         & 0x07FF);
      m_raw_channels[2]  = ((buf[3]>>6 |buf[4]<<2 |buf[5]<<10)   & 0x07FF);
      m_raw_channels[3]  = ((buf[5]>>1 |buf[6]<<7)         & 0x07FF);
      m_raw_channels[4]  = ((buf[6]>>4 |buf[7]<<4)         & 0x07FF);
      m_raw_channels[5]  = ((buf[7]>>7 |buf[8]<<1 |buf[9]<<9)    & 0x07FF);
      m_raw_channels[6]  = ((buf[9]>>2 |buf[10]<<6)          & 0x07FF);
      m_raw_channels[7]  = ((buf[10]>>5|buf[11]<<3)                & 0x07FF);
      m_raw_channels[8]  = ((buf[12]   |buf[13]<<8)                & 0x07FF);
      m_raw_channels[9]  = ((buf[13]>>3|buf[14]<<5)                & 0x07FF);
      m_raw_channels[10] = ((buf[14]>>6|buf[15]<<2|buf[16]<<10) & 0x07FF);
      m_raw_channels[11] = ((buf[16]>>1|buf[17]<<7)                & 0x07FF);
      m_raw_channels[12] = ((buf[17]>>4|buf[18]<<4)                & 0x07FF);
      m_raw_channels[13] = ((buf[18]>>7|buf[19]<<1|buf[20]<<9)  & 0x07FF);
      m_raw_channels[14] = ((buf[20]>>2|buf[21]<<6)                & 0x07FF);
      m_raw_channels[15] = ((buf[21]>>5|buf[22]<<3)                & 0x07FF);

      ((buf[23])      & 0x0001) ? m_ch17 = true: m_ch17 = false;
      ((buf[23] >> 1) & 0x0001) ? m_ch18 = true: m_ch18 = false;

      if ((buf[23] >> 3) & 0x0001) {
        m_failsafe = true;
      } else {
        m_failsafe = false;
      }
      buf.clear();      
    }
  }


  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SBUS::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "SBUS_PORT") {
      this->m_port = value;
      handled = true;
    }
    else if(param == "SBUS_MAXVALUE") {
      this->m_max_value = atoi(value);
      handled = true;
    }
    else if(param == "SBUS_MINVALUE") {
      this->m_max_value = atoi(value);
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  // open up the serial port -- it's distinctly C-ish
  struct termios2 attrib;
  m_dev_fd = open(m_port.c_str(), O_RDWR | O_NONBLOCK | O_NOCTTY);
  if (m_dev_fd < 0) {
    cerr << "Failed to open RC serial port " << m_port;
    return false;
  }
  if (ioctl(m_dev_fd, TCGETS2, &attrib) < 0) {
    cerr << "Failed to open RC serial port " << m_port;
    return false;
  }

  // These commands set the serial speed to 100 kbps
  attrib.c_cflag &= ~CBAUD;
  attrib.c_cflag |= BOTHER;
  attrib.c_ispeed = 100000;
  attrib.c_ospeed = 100000;
  
  // set the port to raw mode
  attrib.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  attrib.c_cflag &= ~(CSIZE); 
  attrib.c_cflag |= (PARENB | CSTOPB);    // even parity, two stop bits
  attrib.c_cflag |= (CLOCAL | CREAD | CS8); // ignore modem status lines, enable receiver, 8 bits per byte
  attrib.c_iflag &= ~(IXON | IXOFF | IXANY);  // turn off all flow control
  attrib.c_iflag &= ~(ICRNL);         // turn off line ending translation         
  attrib.c_oflag &= ~(OPOST);         // turn off post processing of output
  attrib.c_cc[VMIN] = 0;            // this sets the timeouts for the read() operation to minimum
  attrib.c_cc[VTIME] = 1;
  if (ioctl(m_dev_fd, TCSETS2, &attrib) < 0) {
    cerr << "Failed to configure RC serial port " << m_port;
    close(m_dev_fd);
    return false;
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void SBUS::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SBUS::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




