#ifndef ZSimCom
#define ZSimcom
// anything that actually writes to the serial port should be in here

#if GprsPresent > None
  #ifndef GprsBaud
      #if GprsPresent == Sim800
        #define GprsBaud 115200
      #else
        #define GprsBaud 9600
      #endif
  #endif 
  #if GprsPresent == GprsSlave
    #define Delay delay
  #else
    #define Delay Alarm.delay 
  #endif
   
  #include <Arduino.h>
  #define DefTimeout 5000 // 5 sec
  
  enum DataType {
    CMD     = 0,
    DATA    = 1,
  };

 
 #include <Gprs/Simcom/SimcomPins.h>

/*************** end wierd stuff ****/
  bool SimcomReset()
  {
    // Keep reset high
    pinMode(ResetPin, OUTPUT);
    digitalWrite(ResetPin,LOW);
    Delay(1000);
    digitalWrite(ResetPin,HIGH);
    Delay(100);
  }
  

  int SimcomAvailable(void)
  {
    return SimcomSerial.available();
  }

  void SimcomInit(long baudrate)
  {
    
    SimcomSerial.end();
    Dbg"Begin","SimComSerialinit",1);
    Dbg"OnOff",OnOffPin,0);Dbg" Reset",ResetPin,0);Dbg" Power",PowerStatusPin,0);Dbg" RX",GprsRx,0);Dbg" TX",GprsTx,1);
    #if IsEsp32 > 0 || GprsPresent == Gprsslave
      SimcomSerial.begin(baudrate,SERIAL_8N1,GprsRx,GprsTx);
    #else 
      SimcomSerial.begin(baudrate);
    #endif    
    Dbg"Waiting for","SimcomSerial",1);
    while (!SimcomSerial);
    Dbg"SimcomSerial @",baudrate,1);
    // if slave start Serial1
    #if GprsPresent == GprsSlave
      Serial1.end();
      Serial1.begin(baudrate);
      while(!Serial1);
      Dbg"Slave Serial1@",baudrate,1);
    #endif
  }

  void SimcomFlushSerial()
  {
    while(SimcomSerial.available())
    {
        SimcomSerial.read();
    }
  }

  void SimcomReadBuffer(char *buffer, int count, int timeout)
  {
    int i = 0;
    unsigned long timerStart, prevChar;
    timerStart = millis();
    prevChar = 0;
    while(1) 
    {
        while (SimcomSerial.available()) 
        {
            char c = SimcomSerial.read();
            prevChar = millis();
            buffer[i++] = c;
            if (i >= count) break;
        }
        if (i >= count) break;
        if ((unsigned long) (millis() - timerStart) > timeout) break;
    }
    SimcomFlushSerial();
  } 

  void SimcomReadBuffer(char *buffer, int count)
  {
    int i = 0;
    unsigned long timerStart, prevChar;
    timerStart = millis();
    prevChar = 0;
    while(1) 
    {
        while (SimcomSerial.available()) 
        {
            char c = SimcomSerial.read();
            prevChar = millis();
            buffer[i++] = c;
            if (i >= count) break;
        }
        if (i >= count) break;
        if ((unsigned long) (millis() - timerStart) > DefTimeout) break;
    }
    SimcomFlushSerial();
  }

  void SimcomCleanBuffer(char *buffer, int count)
  {
 //   Dbg"Cleanbuffer",sizeof(buffer),1);
    memset(buffer,'\0',count);
  }

  void SimcomSendByte(uint8_t data)
  {
    SimcomSerial.write(data);
  }

  void SimcomSendCmd(const char* cmd)
  {
    for(uint16_t i=0; i<strlen(cmd); i++)
    {
      SimcomSendByte(cmd[i]);
    }
  }

  void SimcomSendCmd(const __FlashStringHelper* cmd)
  {
    int i = 0;
    const char *ptr = (const char *) cmd;
    while (pgm_read_byte(ptr + i) != 0x00) 
    {
      SimcomSendByte(pgm_read_byte(ptr + i++));
    }
  }

  void SimcomSendCmd_P(const char* cmd)
  {
    while (pgm_read_byte(cmd) != 0x00)
    {
      SimcomSendByte(pgm_read_byte(cmd++));
    }
  }

  bool SimcomWaitForResp(const char* resp, DataType type, unsigned long Timeout)
  {
    int len = strlen(resp);
    int sum = 0;
    unsigned long timerStart;
    unsigned long timerEnd;
    timerStart = millis();
    timerEnd = timerStart;
    while((unsigned long) (timerEnd - timerStart) < Timeout) 
    {
      timerEnd = millis();
      if(SimcomSerial.available()) 
      {
        char c = SimcomSerial.read();
        sum = (c==resp[sum]) ? sum+1 : 0;
        if(sum == len) 
        {
          //If is a CMD, we will finish to read buffer.
          if(type == CMD) SimcomFlushSerial();
          return true;
        }
      }
    }
    return false;
  }

  bool SimcomCheckWithCmd(char * cmd, const char *resp, DataType type)
  {
    SimcomSendCmd(cmd); 
    return SimcomWaitForResp(resp,type,DefTimeout);
  }

  bool SimcomCheckWithCmd(char * cmd, const char *resp, DataType type,unsigned long Timeout)
  {
    SimcomSendCmd(cmd); 
    return SimcomWaitForResp(resp,type,Timeout);
  }

  bool SimcomCheckWithCmd(const __FlashStringHelper* cmd, const char *resp, DataType type)
  {
    SimcomSendCmd(cmd); 
    return SimcomWaitForResp(resp,type,DefTimeout);
  }

  bool SimcomCheckWithCmd(const __FlashStringHelper* cmd, const char *resp, DataType type,unsigned long Timeout)
  {
    SimcomSendCmd(cmd); 
    return SimcomWaitForResp(resp,type,Timeout);
  }

  void SimcomSendEndMark(void)
  {
    SimcomSendByte((char)26);
  }

  bool SimcomIsUp()
  {
     return SimcomCheckWithCmd(F("AT\r\n"),"OK",CMD);
  }

  void RebootSlave()
  {
    // either pull slave reset or send "~" via serial1
    SimcomSendCmd("~\R\N");
    Delay(5000); // wait 5 secs to give slave time to boot.
  }


bool SimcomPreInit(void)
{
    Dbg"Begin","SimcomPreinit",1);
    pinMode(PowerStatusPin,INPUT);  // pin 12
    delay(10);
    if(LOW == digitalRead(PowerStatusPin))
    {
        if(!SimcomIsUp())
        {
          Dbg"Bounce OnOff",OnOffPin,1);
            delay(800);  
            digitalWrite(OnOffPin,HIGH); // pin 9
            delay(200);
            digitalWrite(OnOffPin,LOW);
            delay(2000);
            digitalWrite(OnOffPin,HIGH);
  //      delay(5000);  
        }
        while(!SimcomIsUp())
	{
	  Dbg"Checking","Simcom",1);
	  delay(1000);
	}                
        return true;
    }
    else
    {
        Dbg"PowerStatusPin","high",1);
        return true;  
    }
}

  bool SimcomPowerUp()
  {
    Dbg"Simcom","PowerUp",1);
    if (SimcomIsUp()) 
    {
      Dbg"Power","O.K!",1);
      return true;
    }
    // on leo
    //PowerStatusPin 12
    //OnOffPin 9
    //ResetPin 5  

    Dbg"OnOffPin",OnOffPin,1);
    Dbg"PowerStatusPin",PowerStatusPin,1);
    pinMode(OnOffPin, OUTPUT);
    pinMode(PowerStatusPin, INPUT);
    delay(500);
    int x = 3;
    while(x > 0 && LOW == digitalRead(PowerStatusPin))
    {
 //     SimcomReset();
 //     digitalWrite(PowerStatusPin, HIGH); 
      Delay(100);
      digitalWrite(OnOffPin,HIGH); 
      Delay(200);
      digitalWrite(OnOffPin,LOW);
      Delay(1000);
      digitalWrite(OnOffPin,HIGH);
      Delay(500);

      if (SimcomIsUp()) break;
      else
      {
        x--;       
        Dbg"PowerUp","Retry",1);
      }     
    } 
    
    if (x > 0)
    {
      Dbg"Power","O.K!",1);   
      return true;        
    }
    else 
    {
      Dbg"Power","Failed",1);   
      return false;        
    }  
  }


  void SlaveListenForCommands()
  {
    Dbg"Slave","Listening",1);
    char c[0];
    while(1) 
    {       
      if(SimcomSerial.available())
      {
        Serial1.write(SimcomSerial.read());
      }

      if(Serial1.available())
      {    
        c[0] = Serial1.read();
	Dbg"From Master",c[0],1);
//        if (strcmp("|",c) == 0 ) 
//        {
//          Dbg"Calling","PowerUp",1);
//          if (SimcomPowerUp())
//	  {
//           Serial1.write("OK\r\n");
//          //Dbg"Number",,1);  AT+Cnum
//           continue;
//          }
//       }
        if (strcmp("~",c) == 0 ) 
        {
          Dbg"Calling","Reset",1);
          if (SimcomReset())
	  {
           Serial1.write("OK\r\n");
           continue;
          }
        }
        SimcomSerial.write(c[0]); 
      }
    }
  }

  void MasterListenForResponse()
  {
    while(1) 
    {       
      if(SimcomSerial.available())
      {
         DbgPort.write(SimcomSerial.read());  
      }
      if(DbgPort.available())
      {    
        SimcomSerial.write(DbgPort.read()); 
      }
    }
  }

#endif
#endif