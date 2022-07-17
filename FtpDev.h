
String GetMyIp();  //zenwifi.h

#define FTP_SERVER_VERSION "1"
#define FTP_CTRL_PORT    21          // Command port on wich server is listening  
#define FTP_DATA_PORT_PASV 50009     // Data port in passive mode
#define FTP_TIME_OUT  5 * 60      // Disconnect client after 5 minutes of inactivity
#define FTP_AUTH_TIME_OUT 10      // Wait for authentication for 10 seconds
#define FTP_CMD_SIZE FF_MAX_LFN+8 // max size of a command
#define FTP_CWD_SIZE FF_MAX_LFN+8 // max size of a directory name
#define FTP_FIL_SIZE FF_MAX_LFN   // max size of a file name
#define FTP_BUF_SIZE 1024 // 512  // size of file buffer for read/write

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
      #if BoardType >= Esp8266 && BoardType <= EspEnd
        ftpServer.setNoDelay( true );
      #endif
      dataServer.begin();
      delay(10);
      FtpStatus = FtpReset;
   //   iniVariables();
    }

//reset,waiting,idle,active
  void handleFTP()
{
  if (TimedOut(millisDelay)) return;
 // WiFiClient Zclient;
  switch(FtpStatus)
  {
    case  FtpReset:  
    {
      if( Zclient.connected()) disconnectClient(Zclient);
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
//    #ifdef ESP8266  
      if( ftpServer.hasClient())
      {
        Zclient.stop();
        Zclient = ftpServer.available();
      }
//    #else
//    client = ftpServer.accept(); ??? ,available()
//    #endif
      if( Zclient.connected())             // A client connected
      {
        ClientConnected(Zclient);
        millisEndConnection = millis() + 1000L * FTP_AUTH_TIME_OUT; // wait client id for 10 s.
        FtpStatus = FtpUser; 
      }
      break;
    }
    default:
      String Irec = ReadInput(Zclient);
      if (Irec != "")           // got response
      {
         String Cmd = GetElement(Irec,0,' ');
         String Param = GetElement(Irec,1,' ');
         Dbg(Cmd,Param,1);
         ProcessCommand(Zclient,Cmd,Param);
         if(FtpStatus == FtpReset ) millisEndConnection = millis() + 1000L * FTP_AUTH_TIME_OUT;  // wait authentication for 10 s.
          else if( FtpStatus < FtpActive ) millisDelay = millis() + 200;     // delay of 100 ms
               else millisEndConnection = millis() + 1000L * FTP_TIME_OUT;
      }
      if( !Zclient.connected() )  FtpStatus = FtpWaiting;
  }

/*******/
/*
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
    millisDelay = millis() + 200;       // delay of 200 ms
    FtpStatus = FtpReset;
  }
*/
}
 
  private:

    void SetTimer(uint32_t ztimer,uint32 interval)
    {  // wont work need handle of timer
      ztimer = millis() + interval;
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

    void disconnectClient(WiFiClient ZClient)
    {
      Dbg("Disconnecting", "Client", 1);
  //    abortTransfer();
      ZClient.println("221 Goodbye");
      ZClient.stop();
    }

    bool QUIT(WiFiClient Zclient)  //  QUIT
    {
      disconnectClient(Zclient);
      FtpStatus = FtpReset;
    }

   void ResetConnection(WiFiClient Zclient,int ToStatus)
   {
     FtpStatus = ToStatus;
    // if FtpStatus == Ftpreset Quit(ZClient);
   }

   bool ProcessCommand(WiFiClient ZClient,String cmd,String param)
    {					// = needed
      if (cmd == "CDUP") CDUP(ZClient);
      else if (cmd == "USER") return USER(ZClient,param);
      else if (cmd == "PASS") return PASS(ZClient,param);
      else 
      {
        ZClient.println("500 Unknown Command");
        Dbg("Unknown Command",cmd,1);
        FtpStatus = FtpIdle;
        return false;
      }
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

    void CDUP(WiFiClient ZClient)  //  CDUP - Change to Parent Directory
    {
      ZClient.println("250 Ok. Current directory is " + Cwd);
    }


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
 WiFiClient Zclient;

  WiFiClient client;     // eventually remove it
  WiFiClient data;
  uint32_t millisDelay,               //
           millisEndConnection,       // 
           millisBeginTrans,          // store time of beginning of a transaction
           bytesTransfered;           //
};