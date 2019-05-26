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

SBUS::SBUS() {
    m_dev_fd = -1;
    m_errorFrames = 0;
    m_goodFrames = 0;
    m_max_val = 2000;
    m_min_val = 1000;
    m_med_val = 1500;
    m_port = "/dev/ttyO4";
    for (int i = 0; i < RC_CHANNEL_COUNT; i++) {
        m_scaled_channels[i] = 0.0;
        m_raw_channels[i] = 0;
    }
    cerr << "SBUS object created" << endl;
}

//---------------------------------------------------------
// Destructor

SBUS::~SBUS() {
    close(m_dev_fd);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SBUS::OnNewMail(MOOSMSG_LIST &NewMail) {
    AppCastingMOOSApp::OnNewMail(NewMail);
    MOOSMSG_LIST::iterator p;
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
        string key    = msg.GetKey();
        if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
            reportRunWarning("Unhandled Mail: " + key);
    }
    return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SBUS::OnConnectToServer() {
    registerVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SBUS::Iterate() {
    AppCastingMOOSApp::Iterate();
    char buf[SBUS_BUF_LEN];
    ssize_t bytesRead = read(m_dev_fd, buf, SBUS_BUF_LEN);

    while (bytesRead > 0) {
        for (int i = 0; i < bytesRead; i++) m_buf.push_back(buf[i]); // we have to do this in order to copy /0 correctly
        //cerr << "Read " << to_string(bytesRead) << " bytes into m_buf: [" << m_buf << "]" << endl;
        memset(buf, 0, SBUS_BUF_LEN);
        bytesRead = read(m_dev_fd, buf, SBUS_BUF_LEN);
    }

    if (m_buf.size() >= m_buf.rfind(SBUS_STARTBYTE) + SBUS_BUF_LEN) {
        if (m_buf[(m_buf.find(SBUS_STARTBYTE) + (SBUS_BUF_LEN - 1))] != SBUS_ENDBYTE) {
            cerr << "Received invalid frame: [" << m_buf << "]" << endl;
            m_buf.clear();
            m_errorFrames++;
            m_valid = false;
            return false;
        } else {
            m_goodFrames++;
            m_valid = true;

            m_raw_channels[0]  = ((m_buf[1]    |m_buf[2]<<8)                & 0x07FF);
            m_raw_channels[1]  = ((m_buf[2]>>3 |m_buf[3]<<5)                & 0x07FF);
            m_raw_channels[2]  = ((m_buf[3]>>6 |m_buf[4]<<2 |m_buf[5]<<10)  & 0x07FF);
            m_raw_channels[3]  = ((m_buf[5]>>1 |m_buf[6]<<7)                & 0x07FF);
            m_raw_channels[4]  = ((m_buf[6]>>4 |m_buf[7]<<4)                & 0x07FF);
            m_raw_channels[5]  = ((m_buf[7]>>7 |m_buf[8]<<1 |m_buf[9]<<9)   & 0x07FF);
            m_raw_channels[6]  = ((m_buf[9]>>2 |m_buf[10]<<6)               & 0x07FF);
            m_raw_channels[7]  = ((m_buf[10]>>5|m_buf[11]<<3)               & 0x07FF);
            m_raw_channels[8]  = ((m_buf[12]   |m_buf[13]<<8)               & 0x07FF);
            m_raw_channels[9]  = ((m_buf[13]>>3|m_buf[14]<<5)               & 0x07FF);
            m_raw_channels[10] = ((m_buf[14]>>6|m_buf[15]<<2|m_buf[16]<<10) & 0x07FF);
            m_raw_channels[11] = ((m_buf[16]>>1|m_buf[17]<<7)               & 0x07FF);
            m_raw_channels[12] = ((m_buf[17]>>4|m_buf[18]<<4)               & 0x07FF);
            m_raw_channels[13] = ((m_buf[18]>>7|m_buf[19]<<1|m_buf[20]<<9)  & 0x7FF);
            m_raw_channels[14] = ((m_buf[20]>>2|m_buf[21]<<6)               & 0x07FF);
            m_raw_channels[15] = ((m_buf[21]>>5|m_buf[22]<<3)               & 0x07FF);

            ((m_buf[23])      & 0x0001) ? m_ch17 = true: m_ch17 = false;
            ((m_buf[23] >> 1) & 0x0001) ? m_ch18 = true: m_ch18 = false;

            if ((m_buf[23] >> 3) & 0x0001) {
                m_failsafe = true;
            } else {
                m_failsafe = false;
            }
            m_buf.clear();

            for (int i = 0; i < RC_CHANNEL_COUNT; i++) {
                m_scaled_channels[i] = 0.0;
                if ((m_raw_channels[i] < m_min_val) || (m_raw_channels[i] > m_max_val)) {
                    cerr << "Channel " << to_string(i) << " is out of range" << endl;
                    m_scaled_channels[i] = 0.0;
                } else {
                    m_scaled_channels[i] = ((float)((int)m_raw_channels[i] - m_med_val))/(m_max_val - m_med_val);
                }
            }
        }
    }

    if (m_valid) {
        std::string chans = "[";
        std::string scaled_chans = "[";
        for (int i = 0; i < RC_CHANNEL_COUNT; i++) {
            if (i != 0) {
                chans += ", ";
                scaled_chans += ", ";
            }
            chans += to_string(m_raw_channels[i]);
            scaled_chans += to_string(m_scaled_channels[i]);
            Notify("SBUS_Ch" + to_string(i + 1) + "_RAW", m_raw_channels[i]);
            Notify("SBUS_Ch" + to_string(i + 1) + "_SCALED", m_scaled_channels[i]);
        }
        chans = "]";
        scaled_chans = "]";
        m_json_output = "{\"proportional\":";
        m_json_output += chans;
        m_json_output += ",\"scaled\":";
        m_json_output += scaled_chans;
        m_json_output += ",\"digital17\":";
        if (m_ch17) {
            m_json_output += "true";
        } else {
            m_json_output += "false";
        }
        m_json_output += ",\"digital18\":";
        if (m_ch18) {
            m_json_output += "true";
        } else {
            m_json_output += "false";
        }
        m_json_output += ",\"failsafe\":";
        if (m_failsafe) {
            m_json_output += "true";
        } else {
            m_json_output += "false";
        }
        m_json_output += "}";
        Notify("SBUS_Channels", chans);
        Notify("SBUS_Scaled_Channels", scaled_chans);
        Notify("SBUS_Ch17", m_ch17);
        Notify("SBUS_Ch18", m_ch18);
    } else {
        m_failsafe = false;
    }
    Notify("SBUS_Failsafe", m_failsafe);
    Notify("SBUS_GoodFrames", m_goodFrames);
    Notify("SBUS_BadFrames", m_errorFrames);

    AppCastingMOOSApp::PostReport();
    return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SBUS::OnStartUp() {
    cerr << "Configuring pSBUS..." << endl;
    AppCastingMOOSApp::OnStartUp();
    cerr << "Performed automatic configuration for pSBUS..." << endl;

    STRING_LIST sParams;
    m_MissionReader.EnableVerbatimQuoting(false);
    if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
        reportConfigWarning("No config block found for " + GetAppName());
    }
    cerr << "Read in SBUS configuration" << endl;

    STRING_LIST::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
        string orig  = *p;
        string line  = *p;
        string param = toupper(biteStringX(line, '='));
        string value = line;

        bool handled = false;
        if(param == "SBUS_PORT") {
            cerr << "Setting port to " << value << endl;
            this->m_port = value;
            handled = true;
        } else if(param == "SBUS_MAXVALUE") {
            cerr << "Setting max value to " << value << endl;
            this->m_max_val = atoi(value.c_str());
            handled = true;
        } else if(param == "SBUS_MINVALUE") {
            cerr << "Setting min value to " << value << endl;
            this->m_min_val = atoi(value.c_str());
            handled = true;
        }
        if(!handled)
            reportUnhandledConfigWarning(orig);

    }

    m_med_val = (m_max_val + m_min_val)/2;

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
        cerr << "Failed to configure RC serial port " << m_port << endl;
        close(m_dev_fd);
        return false;
    }

    registerVariables();
    return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void SBUS::registerVariables() {
    AppCastingMOOSApp::RegisterVariables();
    // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SBUS::buildReport() {
    m_msgs << "============================================ \n";
    m_msgs << "File: pSBUS                                  \n";
    m_msgs << "============================================ \n";

    ACTable lowchans(8);
    ACTable highchans(8);
    ACTable bools(5);

    for (int i = 0; i < 8; i++) {
        string entry = "CH" + to_string(i+1);
        lowchans << entry;
    }
    for (int i = 8; i < 16; i++) {
        string entry = "CH" + to_string(i+1);
        highchans << entry;
    }
    bools << "CH17 | CH18 | Failsafe | Good Frames | Bad Frames";
    lowchans.addHeaderLines();
    highchans.addHeaderLines();
    bools.addHeaderLines();

    for (int i = 0; i < 8; i++) {
        lowchans << to_string(m_raw_channels[i]);
        highchans << to_string(m_raw_channels[i+8]);
    }
    for (int i = 0; i < 8; i++) {
        lowchans << to_string(m_scaled_channels[i]);
        highchans << to_string(m_scaled_channels[i+8]);
    }
    if (m_ch17) {
        bools << "true";
    } else {
        bools << "false";
    }
    if (m_ch18) {
        bools << "true";
    } else {
        bools << "false";
    }
    if (m_failsafe) {
        bools << "true";
    } else {
        bools << "false";
    }
    bools << to_string(m_goodFrames);
    bools << to_string(m_errorFrames);

    m_msgs << lowchans.getFormattedString() << endl << endl;
    m_msgs << highchans.getFormattedString() << endl << endl;
    m_msgs << bools.getFormattedString() << endl;

    return(true);
}
