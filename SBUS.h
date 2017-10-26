/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SBUS.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef SBUS_HEADER
#define SBUS_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#define RC_CHANNEL_COUNT 	(16)
#define SBUS_BUF_LEN		(25)
#define SBUS_STARTBYTE		(0x0f)
#define SBUS_ENDBYTE		(0x00)

class SBUS : public AppCastingMOOSApp
{
 public:
   SBUS();
   ~SBUS();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   bool readInputs();
   std::string packJSON();

 private: // Configuration variables
 	std::string     m_port;
 	uint16_t        m_max_val;
 	uint16_t        m_min_val;
 	uint16_t        m_med_val;

 private: // State variables
 	std::string            m_json_output;
 	std::string            m_buf;
 	int                    m_dev_fd;
	std::vector<uint16_t>  m_raw_channels { RC_CHANNEL_COUNT };
	std::vector<double>    m_scaled_channels { RC_CHANNEL_COUNT };
	int                    m_errorFrames;
	int                    m_goodFrames;
 	bool                   m_ch17;
 	bool                   m_ch18;
 	bool                   m_failsafe;
 	bool                   m_valid; 
};

#endif 
