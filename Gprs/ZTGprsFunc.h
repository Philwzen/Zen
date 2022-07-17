#ifndef ZGprs
#define ZGprs
// Gprs functionality

#if GprsPresent > None
  #define SimErrorLed   RedLed     // ztgprsfunc.h
  // SimModule dependant stuff
  #include <Gprs/Simcom/ZSimCom.h> 

  bool GprsValidUser(String phone);
  void ProcessSms(int SmsId);
  void ProcessCall();

  bool GprsCheckSimStatus(void)
  {
    char gprsBuffer[32];
    int count = 0;
    SimcomCleanBuffer(gprsBuffer,32);
    while(count < 3) 
    {
        SimcomSendCmd(F("AT+CPIN?\r\n"));
        SimcomReadBuffer(gprsBuffer,32);
        //Serial.println(gprsBuffer);
        if((NULL != strstr(gprsBuffer,"+CPIN: READY"))) break;
        count++;
        delay(300);
    }
    if (count == 3)  return false;
    else return true;
  }

  bool GprsSendSms(char *number, char *data)
  {
    //char cmd[32];
    if(!SimcomCheckWithCmd(F("AT+CMGF=1\r\n"), "OK\r\n", CMD)) 
    { // Set message mode to ASCII
      return false;
    }
    delay(500);
    SimcomFlushSerial();
    SimcomSendCmd(F("AT+CMGS=\""));
    SimcomSendCmd(number);
    //sprintf(cmd,"AT+CMGS=\"%s\"\r\n", number);
    //snprintf(cmd, sizeof(cmd),"AT+CMGS=\"%s\"\r\n", number);
    //if(!SimcomCheckWithCmd(cmd,">",CMD)) {
    if(!SimcomCheckWithCmd(F("\"\r\n"),">",CMD)) 
    {
      return false;
    }
    delay(1000);
    SimcomSendCmd(data);
    delay(500);
    SimcomSendEndMark();
    return SimcomWaitForResp("OK\r\n", CMD,DefTimeout);
  }

  bool GprsGetSms(char *message,int SmsId,int length)
  {   // Return the whole sms string
    int i = 0;
    char gprsBuffer[144]; // Buffer size for the SMS message
    //SimcomCleanBuffer(gprsBuffer,144);
    char cmd[16];
    char *p,*s;
    sprintf(cmd,"AT+CMGR=%d,0\r\n",SmsId);
    SimcomSendCmd(cmd);  // 1

    SimcomCleanBuffer(gprsBuffer,144);
    SimcomReadBuffer(gprsBuffer,144);
    if(NULL != ( s = strstr(gprsBuffer,"+CMGR"))){
        if(NULL != ( s = strstr(gprsBuffer,"REC"))){  // beginning the SMS message
            p = s - 1;
            while((i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
        }
    }
    return true;   
  }

  bool GprsDeleteSms(int index)
  {
    #ifdef NoSim
      return true; 
    #endif
    //AT+CMGd=1,0 delete sms 1
    //AT+CMGd=1,4 delete all sms
  
    if (index == 0)
    {
      SimcomSendCmd("AT+CMGDA=\"DEL ALL\"\r\n");
    }
    else
    {     
      char cmd[16];
      snprintf(cmd,sizeof(cmd),"AT+CMGD=%d\r\n",index);
      SimcomSendCmd(cmd); //2
    }
    // wait OK response  may take a while so timeout 10s
    return SimcomCheckWithCmd(F("\r"),"OK\r\n",CMD,10000);	
  }

  void GprsCommand(char * cmd)
  {
    #ifdef NoSim
      return;
    #endif
    SimcomSendCmd(cmd);
  }

  bool GprsCommandResponse(char * cmd,char * resp)
  {
    #ifdef NoSim
      return false;
    #endif                          // type CMD = 0, DATA = 1,
    #if GprsPresent == GprsMaster 
      return SimcomCheckWithCmd(cmd,resp,CMD,30000);
    #else
      return SimcomCheckWithCmd(cmd,resp,CMD);
    #endif
  }

  bool GprsDataResponse(char * cmd,char * resp)
  {
    #ifdef NoSim
      return false;
    #endif                          // type CMD = 0, DATA = 1,
    #if GprsPresent == GprsMaster 
      return SimcomCheckWithCmd(cmd,resp,DATA,30000);
    #else
      return SimcomCheckWithCmd(cmd,resp,DATA);
    #endif
  }

  void GprsDeleteCalls()
  {
    // does nothing yet
  }

  time_t GprsSetTime()
  {
    #ifdef NoSim
      return now();
    #endif
    unsigned int hr; unsigned int mn; unsigned int sec;
    unsigned int dy; unsigned int mnth; unsigned int yr;
    char GprsResult[100];
    char* cmdbegin;
    char Scratch[3];
    memset(GprsResult,char(0),sizeof(GprsResult));
    if(!GprsDataResponse("AT+CCLK?\r\n","+CCLK")) 
    {
      Dbg("Cannot Get","Network Time",1); 
      return now();
    }
    delay(1000);
    SimcomReadBuffer(GprsResult, sizeof(GprsResult));
    Dbg("GprsTime",GprsResult,1);
    cmdbegin = strstr(GprsResult, "\"");
    memset(Scratch,char(0),3); strncpy(Scratch,cmdbegin + 1,2);
    yr = atoi(Scratch);   cmdbegin = cmdbegin + 3;
    memset(Scratch,char(0),3); strncpy(Scratch,cmdbegin + 1,2);
    mnth = atoi(Scratch); cmdbegin = cmdbegin + 3;
    memset(Scratch,char(0),3); strncpy(Scratch,cmdbegin + 1,2);
    dy = atoi(Scratch);   cmdbegin = cmdbegin + 3;
    memset(Scratch,char(0),3); strncpy(Scratch,cmdbegin + 1,2);
    hr = atoi(Scratch);   cmdbegin = cmdbegin + 3;
    memset(Scratch,char(0),3); strncpy(Scratch,cmdbegin + 1,2);
    mn = atoi(Scratch);   cmdbegin = cmdbegin + 3;
    memset(Scratch,char(0),3); strncpy(Scratch,cmdbegin + 1,2);
    sec = atoi(Scratch);  
    //cmdbegin = cmdbegin + 3;
    setTime(hr,mn,sec,dy,mnth,yr); 
    return now();
  }

  bool ExtractCommand(String msgtxt,int msgtype)
  {
    User    = "";
    Phone   = "invalid";
    Command = "null";
    Param   = "2000";
    msgtxt = SUnQuote(msgtxt);
    msgtxt.toLowerCase();
    Dbg("Received Type",msgtype,1);
    switch(msgtype)
    {
      case 1: // from incommingcall +cmt SMS
      {
        Command = GetElement(msgtxt,3,':');
        Command = Command.substring(7,50);
        msgtxt  = GetElement(msgtxt,1,':');
        User    = GetElement(msgtxt,1,',');
        Phone   = GetElement(msgtxt,0,',');
        break;
      } 
      case 2: // from getmessage
      {
        User    = GetElement(msgtxt,2,',');
        Phone   = GetElement(msgtxt,1,',');
        Command = GetElement(msgtxt,4,',');
        Command = GetElement(Command,1,'+');
        Command.remove(0,2); // remove leading crap
        Command.remove(Command.length() - 5 ,5); // and trailing crap
        break;
      }
      case 3: // incomming call RING
      {
        String message;
        message = GetElement(msgtxt,1,':'); // remove rubbish from beginning
        Phone = SUnQuote(GetElement(message,0,','));
        User  = SUnQuote(GetElement(message,4,','));
        Command = "RING";
        GprsCommand("ATH\r\n");
        break;
      }
    }
    Command.trim();
   Dbg("align","12345678901234567890",1);
   Dbg("align",msgtxt,1);
   Dbg("User",User,1);
   Dbg("Phone",Phone,1);
   Dbg("Command",Command,1);

    #if FileSys > None
      //LogIt("","Command Received",Command,User,Phone);
    #endif

    if (User == "") 
      if (!ValidUser(Phone)) 
      {
        LogIt("","Invalid User",Command,User,Phone);
        return false;
      }

    if (Command.indexOf("@") > -1)
    {
      Param   = GetElement(Command,1,'@');
      Command = GetElement(Command,0,'@');
      Dbg(Command,Param,1);
    }

    return true;
  }

  int GprsReadable(void)
  {
    return SimcomAvailable();
  }

  bool GetMessage(int SmsId)   // should never get called !!!
  {
    if (SmsId == 0 ) return false;
    //|| SmsId == 255
    char GprsResult[144];
    memset(GprsResult,char(0),sizeof(GprsResult));
    // retrieve from sim 
    GprsGetSms(GprsResult,SmsId,sizeof(GprsResult));
    // Dbg("SMS",GprsResult,1);
    // populate command etc
    return ExtractCommand(ToString(GprsResult),2);
  }

  bool GprsInComing()
  {
    #ifdef NoSim
      return false;
    #endif
    char GprsResult[144];
    memset(GprsResult,'\0',sizeof(GprsResult));
    SimcomReadBuffer(GprsResult, sizeof(GprsResult) , 1 ); //DEFAULT_TIMEOUT = 5
    Dbg("Incomming",GprsResult,1);
    if(NULL != strstr(GprsResult,"+CMT:"))  // sms received also has $$DST in it so check first
    {
      Dbg("Found","CMT",1);
      bool ok = ExtractCommand(ToString(GprsResult),1);
      memset(GprsResult,'\0',sizeof(GprsResult)); 
      if (ok) 
      {
        ProcessSms(-1);
        return true;
      }
      else 
      {
        Dbg("Rubbish","Disregard",1);
        return false;
      } 
    }
    // if(NULL != strstr(GprsResult,"$$DST:")) return false;
    // if(NULL != strstr(GprsResult,"ATH$$")) return false;
    if(NULL != strstr(GprsResult,"RING")) // call received
    {
      bool ok = ExtractCommand(ToString(GprsResult),3);
      memset(GprsResult,'\0',sizeof(GprsResult));
      if (ok) 
      {
        ProcessCall();
        return true;
      }
      else return false;
    }
//    Dbg("NO","Ring found",1);
    return false;
  }
#if IsEsp32 > 0
  void IRAM_ATTR TidySim()
  {
    SimcomSerial.write("AT+CMGDA=\"DEL ALL\"\r\n");
  }
#else
  void TidySim()
  {
    GprsDeleteSms(0);
    GprsDeleteCalls();
  }
#endif

  void GprsSetDefaults()
  {
    char M1[14] = "0";
    Dbg(F("Setting Sim"),F("Defaults"),1);

    if(!GprsCommandResponse("AT+CLTS=1\r\n","OK")) strcpy(M1,"Network Time"); // use network time
    if(!GprsCommandResponse("AT+CPBS=\"SM\"\r\n","OK")) strcpy(M1,"PhoneBook");  // use sim phone book
    if(!GprsCommandResponse("AT+CLIP=1\r\n","OK")) strcpy(M1,"CallerID");        // use caller id
    if(!GprsCommandResponse("AT+CMGF=1\r\n","OK")) strcpy(M1,"TextMode");       // Set Txt mode SMS
    if(!GprsCommandResponse("AT+CNMI=1,2,0,0,0\r\n","OK")) strcpy(M1,"SMS CMT");  // set +CMT on serial when msg arrrives
    if(!GprsCommandResponse("AT&W\r\n","OK"))  strcpy(M1,"Save Defaults");        // save defaults
    if (M1[0] != '0') Dbg(F("Failed To Set Sim"),M1,1);
    else Dbg(F("Defaults Set"),F("OK"),1);
  }

  bool GprsInit(long baudrate)
  {
    #ifdef NoSim
      return true;
    #endif

    ShowStatus(GetLed(SimErrorLed),1,0);
    Dbg("Powering Up","Modem",1);  
    //delay(5000); // 5secs for slave
    SimcomInit(baudrate);

    #if GprsPresent == GprsMaster 
      pinMode(PowerStatusPin,INPUT);
      Alarm.delay(500);
      while (!digitalRead(PowerStatusPin))
      {
        Dbg("Waiting For","Modem",0);
        Dbg(" On Pin",PowerStatusPin,1);
        yield();
        Alarm.delay(1000);
      }
      Dbg("Modem","Powered Up",1);
      Alarm.delay(500);
    #else
      if(!SimcomIsUp()) 
      {
        if (!SimcomPowerUp())
        {
          Dbg("PowerUp","Failed",1);
          return false;
        }
        digitalWrite(PowerStatusPin, HIGH);
      }
    #endif

    // set phone functionality ?? why bother
    if(!SimcomCheckWithCmd(F("AT+CFUN=1\r\n"),"OK\r\n",CMD))
    {
       Dbg("Simcom","CFUN BAD",1);
       return false;
    }
    
    if(!GprsCheckSimStatus()) 
    {
       Dbg("Simcom","Status BAD",1);
       return false;
    }

    ShowStatus(GetLed(SimErrorLed),0,0);
    Dbg("Sim Ok @",baudrate,1);
    ShowStatus(GetLed(GreenLed),1,0);
    GprsSetDefaults();
    delay(500); 
    ShowStatus(GetLed(GreenLed),0,0);
   // TidySim(); // remove old sms and calls
    return true;
  }

  void GprsGetAllCalls(bool del)
  {
    //  GprsDataResponse("AT+????=ALL\r\n","OK"); // to get all the call records. where to??
    //  char GprsResult[150];  // not big enough !!!!
    //  sim900_readBuffer(GprsResult, sizeof(GprsResult) , DEFAULT_TIMEOUT);
    //  // now what? to sd card ?
    //  memset(GprsResult,'\0',sizeof(GprsResult));
    if (del) GprsDeleteCalls();
  }

  void GprsGetAllSms(bool del)
  {
    GprsDataResponse("AT+CMGL=ALL\r\n","OK"); // to get all the sms records. where to??
    char GprsResult[150];  // not big enough !!!!
    SimcomReadBuffer(GprsResult, sizeof(GprsResult));
    // now what? to sd card ?
   
    memset(GprsResult,'\0',sizeof(GprsResult));
    if (del) GprsDeleteSms(0);
  }

  /*************************** User Stuff **************************/
  String GetAUser(String user)
  {
    char GprsResult[100];
    char GprsCmd[40];
    memset(GprsCmd,'\0',40);
    sprintf(GprsCmd,"AT+CPBF=\"%s\"\r\n",user.c_str());
    memset(GprsResult,'\0',sizeof(GprsResult));
    if (GprsDataResponse(GprsCmd,"+CPBF:"));
    {
      SimcomReadBuffer(GprsResult, sizeof(GprsResult));
    }
    return ToString(GprsResult);
  }

  String GetUsers()
  {  
    char GprsResult[500];
    GprsDataResponse("AT+CPBR=1,250\r\n","+CPBR");
    memset(GprsResult,'\0',sizeof(GprsResult));
    SimcomReadBuffer(GprsResult, sizeof(GprsResult));
    return ToString(GprsResult);
  }

  int GetId(String name)
  {   
     int id = 0;
     char GprsCmd[40];
     char GprsResult[100];
     memset(GprsCmd,'\0',40);
     memset(GprsResult,'\0',sizeof(GprsResult));
     sprintf(GprsCmd,"AT+CPBF=\"%s\"\r\n",name.c_str());
     if (GprsDataResponse(GprsCmd,"+CPBF:"));
     {
       SimcomReadBuffer(GprsResult, sizeof(GprsResult));
       id = GetElement(GprsResult,0,',').toInt();  
     }
     return id;
  }

  bool GprsValidUser(String phone)
  {

    bool ok ;
//return true;
    // test without country code 
    if (phone.startsWith("+679")) phone.remove(0,4);    
    // ok = can find entry in book for phone.  
    // AT+CBPF+" but name needs to be the number !!!!

    ok = (GetId(phone) > 0);
      
 //   #if FileSys > None
 //     if (!ok) 
 //     {        
 //       //LogIt("unknown","Unknown Number",Command,User,Phone);
 //       if (Policy.UseSecurity) 
 //       {
 //         LogIt("","Security","Rejected Invalid User",User,Phone);
 // 	  ok = false;
 //       }
 //       else          
 //       {
 //         LogIt("","Security","Disabled Unknown Allowed",User,Phone);
 //         ok = true; // disable security but log invalid users
 //       }
 //     }
 //     #endif 
    return ok;
  }

  int ExportUsers()
  {
    #if FileSys > None
      FileDelete("UserDump.txt");
      delay(500);
      File file = FileOpenWrite("UserDump.txt");
      String Users = GetUsers();
      Dbg("Users",Users,1);
      int x = 1;
      String Orec;
      String item = GetElement(Users,x,':');
      while ( item != "")
      {
        Orec = GetElement(item,1,',') + "#" + GetElement(item,3,',') + "|";
        FileWriteLine(file,Orec.c_str());
        Dbg("Orec",Orec,1);
        x++;
        item = GetElement(Users,x,':');
      }
      FileClose(file);
      return x - 1;
    #else
      return 0;
    #endif
  }

  bool DeleteById(int id)
  {
    char GprsCmd[40];
    memset(GprsCmd,'\0',40);
    sprintf(GprsCmd,"AT+CPBW=%u\r\n",id);
    Dbg("GprsCommand",GprsCmd,1);
    //  GprsCommandResponse(GprsCmd,"OK");
    GprsCommand(GprsCmd);
    Dbg("Deleted Id",id,1);
    return true;
  }

  int ClearUsers()
  {
    if (!IsAdmin(User)) 
    {
      LogIt("","Security","ClearUsers Rejected Not Admin",User,Phone);
      return 0;
    }  
    char GprsCmd[40];
    String Users = GetUsers();
    Dbg("Users",Users,1);
    int x = 1;
    int id;
    String item = GetElement(Users,x,':');
    Dbg("Item1",item,1);
    while ( item != "")
    {
      id = atoi(GetElement(item,0,',').c_str());
      DeleteById(id);
      x++;
      item = GetElement(Users,x,':');
      delay(1000);
      Dbg("Itemx",item,1); 
    }
    return x -1;
  }

  bool GprsDeleteUser(String Name)
  {
    Dbg("GprsDelete User",Name,1);
    // find entry number by name, then delete by entry number
    // at+cpbf="Phil" returns int location GetID("phil");
    // at+cpbw=int location.
    if (!IsAdmin(User)) 
    {
      LogIt("","GprsDelete User Rejected Not Admin",User,Phone,Name);
      return false;
    }
    int id;
    id = GetId(Name);
    DeleteById(id);
    return true;
  }

  void CreateSimEntry(int type,const char * name,const char * number)
  {
    char GprsCmd[50];
    memset(GprsCmd,'\0',50);
    sprintf(GprsCmd,"AT+CPBW=,\"%s\",%u,\"%s\"\r\n",number,type,name);
    Dbg("CreateSimEntry",GprsCmd,1);
    //  GprsCommandResponse(GprsCmd,"OK");
    GprsCommand(GprsCmd);
    // of the form AT+CPBW=null,num,numtype(int),name
    //  at+cpbw=,"+6799373719",145,"Phil"  write to first free entry.
  }

  bool GprsAddUser(String number,String name)
  {
    // need to store twice once as local number once with int prefix
    if (!IsAdmin(User)) 
    {
      LogIt("","GprsAddUser Rejected Not Admin",User,Phone,Param);
      return false;
    }
    if (name == "") name = number;
    // make 2 entries international and local
    // 129 local 145 international

    if (!number.startsWith("+")) number = "+679" + number;
    name = name + "I";
    CreateSimEntry(145,name.c_str(),number.c_str());
    delay(500);

    number.remove(0,4); //??? what if overseas <3 digit code  
    name.remove(name.length() - 1,1);
    CreateSimEntry(129,name.c_str(),number.c_str());
    return true;
  }

  int LoadUsers(bool ClearAll)
  { 
    if (!IsAdmin(User)) return 0;
    String users = "";
    #if FileSys >  None
      users = FileToString("Users.txt");
    #endif
    //  Dbg("LoadUsers",users,1);
    // chop it up and add to sim card if not there already.
    if (ClearAll && IsAdmin(User) && users != "")
    { // delete users from sim first unless and admin
      Dbg("Deleted",ClearUsers(),1);
    }
    // users =  number#ame|number#name etc etc
    // data  =  number#name
    String data; 
    int x = 0; 
    do
    {
      data = GetElement(users,x,DelimOne); 
      if (data != "")
      {
        String number;
        String name;
        if (data.indexOf(DelimTwo) > 0) 
        {
          number = GetElement(data,0,DelimTwo);
          name   = GetElement(data,1,DelimTwo);
        }
        else 
        {
          number = Param;
          name   = Param;
        }
        AddUser(number,name);
        x++;
        delay(1000);
      }    
    } while (data != "");
    return x;
  }

#else
  bool GetMessage(int SmsId) {}
  bool GprsAddUser(String Param) {}
  bool GprsDeleteUser(String Name) {}
  int LoadUsers(bool ClearAll){}
  int ClearUsers() {}
  int ExportUsers() {}
  void GprsSendSms(char * Phone,char * mes) {}
#endif  // if GprsPresent
#endif // ZGprs