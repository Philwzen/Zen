#define FTP_DEBUG

#ifndef ZFtp
#define ZFtp

String GetMyIp();  //zenwifi.h

#define FTP_SERVER_VERSION "15"
#define FTP_CTRL_PORT    21          // Command port on wich server is listening  
#define FTP_DATA_PORT_PASV 50009     // Data port in passive mode
#define FTP_TIME_OUT  5           // Disconnect client after 5 minutes of inactivity
#define FTP_AUTH_TIME_OUT 10      // Wait for authentication for 10 seconds
#define FTP_CMD_SIZE 255 + 8 // max size of a command
#define FTP_CWD_SIZE 255 + 8 // max size of a directory name
#define FTP_FIL_SIZE 255     // max size of a file name
//#define FTP_BUF_SIZE 1024 //512   // size of file buffer for read/write
#define FTP_BUF_SIZE 2*1460 //512   // size of file buffer for read/write

WiFiServer ftpServer(FTP_CTRL_PORT);
WiFiServer dataServer(FTP_DATA_PORT_PASV);

class FtpServer
{
  public:
    
    void begin(String uname, String pword)
    {
      FTPUSER = uname;
      FTPPASS = pword;
      Dbg("Ftp Server StartUp V", FTP_SERVER_VERSION, 1);
      ftpServer.begin();
      delay(10);
      #if IsEsp8266 > 0 
        ftpServer.setNoDelay( true );
      #endif
      dataServer.begin();
      delay(10);
      millisTimeOut = (uint32_t)FTP_TIME_OUT * 60 * 1000;
      millisDelay = 0;
      FtpStatus = FtpReset;
      iniVariables();
    }

    void handleFTP()
    {
      if (TimedOut(millisDelay)) return;
      // WiFiClient Zclient;
      switch(FtpStatus)
      {
        case  FtpReset:  
        {
          if( client.connected()) disconnectClient(client);
          FtpStatus = FtpWaiting;
          break;
        }
        case FtpWaiting:       // Ftp server waiting for connection
        {
      //    abortTransfer();
      //    iniVariables();
          Dbg("Ftp server waiting on port",FTP_CTRL_PORT,1);
          FtpStatus = FtpIdle;
          break;
        }
        case FtpIdle:     // Ftp server idle
        {
          #if WiFiType == NativeEsp  
            if( ftpServer.hasClient())
            {
              client.stop();
              client = ftpServer.available();
            }
          #else // what to do if not nativeesp???? 
            if (client)
            {
              client.stop();
              client = ftpServer.available();
            }
          #endif
          if(client.connected())             // A client connected
          {
            ClientConnected(client);
            SetEndConnectionTimeOut(FTP_AUTH_TIME_OUT);
            FtpStatus = FtpUser; 
          }
          break;
        }
        default:
        {
          String Irec = ReadInput(client);
          if (Irec != "")           // got response
          {
            String Cmd = GetElement(Irec,0,' ');
            String Param = GetElement(Irec,1,' ');
            Cmd.trim();
            Param.trim();
       //     Dbg(Cmd,Param,1);
            ProcessCommand(client,data,Cmd,Param);
          }
          if( !client.connected() )  FtpStatus = FtpWaiting;
        }  
      } //switch

      if(FtpStatus == FtpReset) 
        SetEndConnectionTimeOut(FTP_AUTH_TIME_OUT);  // wait authentication for 10 s.
      else if( FtpStatus < FtpActive) 
             SetMillisDelay(200);     // delay of 100 ms
           else 
             SetEndConnectionTimeOut(FTP_TIME_OUT);
  
 /*******/
  /* transfer logic not done yet
  if( transferStage == FTP_Retrieve )   // Retrieve data
  {
    if( ! doRetrieve()) transferStage = FTP_Close;
  }
  else if( transferStage == FTP_Store ) // Store data
  {
    if( ! doStore()) transferStage = FTP_Close;
  }
  else if( transferStage == FTP_List ||
           transferStage == FTP_Nlst)   // LIST or NLST
  {
    if( ! doList())  transferStage = FTP_Close;
  }
  else if( transferStage == FTP_Mlsd )  // MLSD listing
  {
    if( ! doMlsd())  transferStage = FTP_Close;
  }
  else if(FtpStatus > FtpIdle && TimedOut(millisEndConnection))
  {
    Zclient.println("530 Timeout");
    SetMillisDelay(200);       // delay of 200 ms
    FtpStatus = FtpReset;
  }
  */
}
  
  private:
    bool ProcessCommand(WiFiClient ZClient,WiFiClient ZData,String cmd,String param)
    {					// = needed
      if (cmd == "CDUP")      return CDUP(ZClient);
      else if (cmd == "USER") return USER(ZClient,param); //
      else if (cmd == "PASS") return PASS(ZClient,param); //     
      else if (cmd == "SYST") return SYST(ZClient);     // 
      else if (cmd == "FEAT") return FEAT(ZClient);
      else if (cmd == "PWD")  return PWD(ZClient);  //
      else if (cmd == "TYPE") return TYPE(ZClient,param);
      else if (cmd == "PASV") return PASV(ZClient,ZData); //
      else if (cmd == "PORT") return PORT(); 
     
      /*    
      else if (cmd == "CWD")  CWD(ZClient,param));  //
      else if (cmd == "QUIT") QUIT(ZClient)); //
      else if (cmd == "MODE") MODE(ZClient,param)); 
      else if (cmd == "STRU") STRU(ZClient,param);
      else if (cmd == "ABOR") ABOR(ZClient,param);
      else if (cmd == "DELE") DELE(ZClient,param); //
      else if (cmd == "LIST") LIST(ZClient,param); //
      else if (cmd == "MLSD") MLSD(ZClient,param);
      else if (cmd == "NLST") NLST(ZClient,param);
      else if (cmd == "NOOP") NOOP(ZClient,param);
      else if (cmd == "RETR") RETR(ZClient,param); //
      else if (cmd == "STOR") STOR(ZClient,param); //
      else if (cmd == "MKD")  MKD(ZClient,param);  // 
      else if (cmd == "RMD")  RMD(ZClient,param);  //
      else if (cmd == "RNFR") RNFR(ZClient,param);
      else if (cmd == "RNTO") RNTO(ZClient,param);
      else if (cmd == "MDTM") MDTM(ZClient,param);
      else if (cmd == "SIZE") SIZE(ZClient,param); //
      else if (cmd == "SITE") SITE(ZClient,param);
      */ 
      else 
      {
        ZClient.println("500 Unknown Command");
        Dbg("Unknown Command",cmd,1);
        FtpStatus = FtpIdle;
        return false;
      }
    }
/******************  Control stuff ********************/
    void iniVariables()
    {
      dataPort = FTP_DATA_PORT_PASV;
      dataPassiveConn = true;
      strcpy(cwdName, "/");
      rnfrCmd = false;
      FtpStatus = 0;
      transferStatus = 0;
    }

    void SetMillisDelay(int var)
    {
        millisDelay = millis() + var;
    }

    void SetEndConnectionTimeOut(int Secs)
    {
        millisEndConnection = millis() + 1000L * Secs;
    }

    bool TimedOut(uint32_t ztimer)
    {
      return (int32_t) (ztimer - millis()) > 0; 
    }
    
    String ReadInput(WiFiClient Zclient)
    {
    String Irec = ""; 
    while (Zclient.connected()) 
    {
      if (Zclient.available()) 
      {
        char c = Zclient.read();
        Serial.write(c);
        Irec = Irec + c;
        if (c == '\n') break;
      }
      else break;
    }
    return Irec;
  }

    void ClientConnected(WiFiClient ZClient)
    {
      Dbg("Client", "Connected!", 1);
      ZClient.println("220--- Welcome to FTP ---");
      ZClient.println("220---   By Myaso   ---");
      ZClient.println("220 --   Version " + String(FTP_SERVER_VERSION) + "   --");
   //   abortTransfer();
   //   iniVariables();
      FtpStatus = FtpIdle;
    }

    void disconnectClient(WiFiClient ZClient)
    {
      Dbg("Disconnecting", "Client", 1);
  //    abortTransfer();
      ZClient.println("221 Goodbye");
      ZClient.stop();
    }

   void ResetConnection(WiFiClient Zclient,int ToStatus)
   {
     FtpStatus = ToStatus;
    // if FtpStatus == Ftpreset Quit(ZClient);
   }
  /************************************************************/

  // implemented FTP Commands

    bool CDUP(WiFiClient ZClient)  //  CDUP - Change to Parent Directory
    {
      ZClient.println("250 Ok. Current directory is " + String(cwdName));
      return true;
    }

    bool QUIT(WiFiClient Zclient)  //  QUIT
    {
      disconnectClient(Zclient);
      FtpStatus = FtpReset;
      return true;
    }

    boolean USER(WiFiClient ZClient,String user)
    {  
      user.trim();
      if (user != FTPUSER)
      {
        ZClient.println("530 user not found");
        ResetConnection(ZClient,FtpReset);
        return false;
      }
      else
      {
        ZClient.println("331 OK. Password required");
        FtpStatus = FtpPass;
        return true;
      }
    }

    boolean PASS(WiFiClient ZClient,String pass)
    {
      pass.trim();
      if (pass != FTPPASS)
      {
          ZClient.println("530");
          QUIT(ZClient);
          return false;
      }
      else
      {
        Dbg("OK", "Waiting for Commands", 1);
        ZClient.println("230 OK.");
	FtpStatus = FtpActive;
        return true;
      }
    }
    bool SYST(WiFiClient ZClient)
    {
      ZClient.println("215 MSDOS");
      return true;
    }   
    bool FEAT(WiFiClient ZClient) // maybe just send 211 and dont  surport anything ??
    {
      ZClient.println("211-Extensions suported:");
      ZClient.println(" MLSD");
      ZClient.println("211 End.");
      return true;
    }
    
    bool PWD(WiFiClient ZClient)   //  PWD - Print Directory
    {
      ZClient.println("257 \"" + String(cwdName) + "\" is your current directory");
      return true;
    }
    
    bool TYPE(WiFiClient ZClient,String Param)
    {
      if (Param == "A")
        ZClient.println("200 TYPE is now ASII");
      else if (Param == "I")
        ZClient.println("200 TYPE is now 8-bit binary");
      else
        ZClient.println("504 Unknow TYPE");
      return true;
    }

    bool PASV(WiFiClient ZClient,WiFiClient ZData)
    {
      if (ZData.connected()) ZData.stop();
      //dataServer.begin();
      dataIp = WiFi.localIP();      
      dataPort = FTP_DATA_PORT_PASV;
      //ZData.connect(dataIp, dataPort);
      //ZData = dataServer.available();
      Dbg("Connection", "Passive ", 0);
      Dbg("Data port", dataPort, 1);
      client.println("227 Entering Passive Mode (" + GetMyIp() + ":" + dataPort + ").");
      dataPassiveConn = true;
      return true;
    }

    bool PORT()
    { // does nothing
      return true;
    }

/**************** Vars etc ***********************/
    IPAddress  dataIp;              // IP address of client for data
  
  // try to get rid of these globals
    WiFiClient client;
    WiFiClient data;

    File file;
    
    enum FtpStatus { FtpReset= 0,   // In this stage, stop any connection
                     FtpWaiting,    //  initialize some variables
                     FtpIdle,       //  wait for client connection
                     FtpUser,       //  wait for user name
                     FtpPass,       //  wait for user password
                     FtpActive };   //  answers to commands

    enum ftpTransfer { FTP_Close = 0, // In this stage, close data channel
                       FTP_Retrieve,  //  retrieve file
                       FTP_Store,     //  store file
                       FTP_List,      //  list of files
                       FTP_Nlst,      //  list of name of files
                       FTP_Mlsd };    //  listing for machine processing

    enum ftpDataConn { FTP_NoConn = 0,// No data connexion
                       FTP_Pasive,    // Pasive type
                       FTP_Active };  // Active type

    int FtpStatus = FtpReset;
    String Cwd = "/";
    String   FTPUSER;
    String   FTPPASS;
    uint16_t dataPort;
    boolean  dataPassiveConn;

    char     buf[ FTP_BUF_SIZE ];       // data buffer for transfers
    char     cmdLine[ FTP_CMD_SIZE ];   // where to store incoming char from client
    char     cwdName[ FTP_CWD_SIZE ];   // name of current directory
    char     command[ 5 ];              // command sent by client
    boolean  rnfrCmd;                   // previous command was RNFR
    char *   parameters;                // point to begin of parameters sent by client
    uint16_t iCL;                       // pointer to cmdLine next incoming char
    int8_t   cmdStatus,                 // status of ftp command connexion
             transferStatus;            // status of ftp data transfer
    uint32_t millisTimeOut,             // disconnect after 5 min of inactivity
             millisDelay,
             millisEndConnection,       //
             millisBeginTrans,          // store time of beginning of a transaction
             bytesTransfered;           
/**********************************************************/ 

    boolean dataConnect()
    {
      unsigned long startTime = millis();
      //wait 5 seconds for a data connection
      if (!data.connected())
      {
 //       while (!dataServer.hasClient() && millis() - startTime < 10000)
        while (millis() - startTime < 10000)   
        {
          //delay(100);
          yield();
        }
 //       if (dataServer.hasClient())
        {
          data.stop();
          data = dataServer.available();
          Dbg("FtpDataServer", "Client", 1);
        }
      }
      return data.connected();
    }

    boolean doRetrieve()
    {
      if (data.connected())
      {
        int16_t nb = file.readBytes(buf, FTP_BUF_SIZE);
        if (nb > 0)
        {
          data.write((uint8_t*)buf, nb);
          bytesTransfered += nb;
          return true;
        }
      }
      closeTransfer();
      return false;
    }

    boolean doStore()
    {
      // Avoid blocking by never reading more bytes than are available
      int navail = data.available();
      if (navail > 0)
      {
        // And be sure not to overflow buf.
        if (navail > FTP_BUF_SIZE) navail = FTP_BUF_SIZE;
        int16_t nb = data.read((uint8_t*) buf, navail);
        if (nb > 0)
        {
          file.write((uint8_t*) buf, nb);
          bytesTransfered += nb;
        }
      }
      if (!data.connected() && (navail <= 0))
      {
        closeTransfer();
        return false;
      }
      else
      {
        return true;
      }
    }

    void closeTransfer()
    {
      uint32_t deltaT = (int32_t) (millis() - millisBeginTrans);
      if (deltaT > 0 && bytesTransfered > 0)
      {
        client.println("226-File successfully transferred");
        client.println("226 " + String(deltaT) + " ms, " + String(bytesTransfered / deltaT) + " kbytes/s");
      }
      else
        client.println("226 File successfully transferred");

      file.close();
      data.stop();
    }

    void abortTransfer()
    {
      if (transferStatus > 0)
      {
        file.close();
        data.stop();
        client.println("426 Transfer aborted");
        Dbg("Transfer", "Aborted!", 1);
      }
      transferStatus = 0;
    }

    // Read a char from client connected to ftp server
    //
    //  update cmdLine and command buffers, iCL and parameters pointers
    //
    //  return:
    //    -2 if buffer cmdLine is full
    //    -1 if line not completed
    //     0 if empty line received
    //    length of cmdLine (positive) if no empty line received

    int8_t readChar()
    {
      int8_t rc = -1;
      if (client.available())
      {
        char c = client.read();
        Dbg("READ", c, 1);
        if (c == '\\')
          c = '/';
        if (c != '\r')
          if (c != '\n')
          {
            if (iCL < FTP_CMD_SIZE)
              cmdLine[ iCL ++ ] = c;
            else
              rc = -2; //  Line too long
          }
          else
          {
            cmdLine[ iCL ] = 0;
            command[ 0 ] = 0;
            parameters = NULL;
            // empty line?
            if (iCL == 0)
              rc = 0;
            else
            {
              rc = iCL;
              // search for space between command and parameters
              parameters = strchr(cmdLine, ' ');
              if (parameters != NULL)
              {
                if (parameters - cmdLine > 4)
                  rc = -2; // Syntax error
                else
                {
                  strncpy(command, cmdLine, parameters - cmdLine);
                  command[ parameters - cmdLine ] = 0;

                  while (* (++ parameters) == ' ')
                    ;
                }
              }
              else if (strlen(cmdLine) > 4)
                rc = -2; // Syntax error.
              else
                strcpy(command, cmdLine);
              iCL = 0;
            }
          }
        if (rc > 0)
          for (uint8_t i = 0 ; i < strlen(command); i ++)
            command[ i ] = toupper(command[ i ]);
        if (rc == -2)
        {
          iCL = 0;
          client.println("500 Syntax error");
        }
      }
      return rc;
    }

    // Make complete path/name from cwdName and parameters
    //
    // 3 possible cases: parameters can be absolute path, relative path or only the name
    //
    // parameters:
    //   fullName : where to store the path/name
    //
    // return:
    //    true, if done

    boolean makePath(char * fullName)
    {
      return makePath(fullName, parameters);
    }

    boolean makePath(char * fullName, char * param)
    {
      if (param == NULL)
        param = parameters;

      // Root or empty?
      if (strcmp(param, "/") == 0 || strlen(param) == 0)
      {
        strcpy(fullName, "/");
        return true;
      }
      // If relative path, concatenate with current dir
      if (param[0] != '/')
      {
        strcpy(fullName, cwdName);
        if (fullName[ strlen(fullName) - 1 ] != '/')
          strncat(fullName, "/", FTP_CWD_SIZE);
        strncat(fullName, param, FTP_CWD_SIZE);
      }
      else
        strcpy(fullName, param);
      // If ends with '/', remove it
      uint16_t strl = strlen(fullName) - 1;
      if (fullName[ strl ] == '/' && strl > 1)
        fullName[ strl ] = 0;
      if (strlen(fullName) < FTP_CWD_SIZE)
        return true;

      client.println("500 Command line too long");
      return false;
    }

    // Calculate year, month, day, hour, minute and second
    //   from first parameter sent by MDTM command (YYYYMMDDHHMMSS)
    //
    // parameters:
    //   pyear, pmonth, pday, phour, pminute and psecond: pointer of
    //     variables where to store data
    //
    // return:
    //    0 if parameter is not YYYYMMDDHHMMSS
    //    length of parameter + space

    uint8_t getDateTime(uint16_t * pyear, uint8_t * pmonth, uint8_t * pday,
                         uint8_t * phour, uint8_t * pminute, uint8_t * psecond)
    {
      char dt[ 15 ];

      // Date/time are expressed as a 14 digits long string
      //   terminated by a space and followed by name of file
      if (strlen(parameters) < 15 || parameters[ 14 ] != ' ')
        return 0;
      for (uint8_t i = 0; i < 14; i++)
        if (!isdigit(parameters[ i ]))
          return 0;

      strncpy(dt, parameters, 14);
      dt[ 14 ] = 0;
      * psecond = atoi(dt + 12);
      dt[ 12 ] = 0;
      * pminute = atoi(dt + 10);
      dt[ 10 ] = 0;
      * phour = atoi(dt + 8);
      dt[ 8 ] = 0;
      * pday = atoi(dt + 6);
      dt[ 6 ] = 0 ;
      * pmonth = atoi(dt + 4);
      dt[ 4 ] = 0 ;
      * pyear = atoi(dt);
      return 15;
    }

    // Create string YYYYMMDDHHMMSS from date and time
    //
    // parameters:
    //    date, time
    //    tstr: where to store the string. Must be at least 15 characters long
    //
    // return:
    //    pointer to tstr

    char * makeDateTimeStr(char * tstr, uint16_t date, uint16_t time)
    {
      sprintf(tstr, "%04u%02u%02u%02u%02u%02u",
               ((date & 0xFE00) >> 9) + 1980, (date & 0x01E0) >> 5, date & 0x001F,
               (time & 0xF800) >> 11, (time & 0x07E0) >> 5, (time & 0x001F) << 1);
      return tstr;
    }
};

#endif 