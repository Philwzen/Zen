#ifndef Zmsg
#define Zmsg

bool SerialOn = false;

#ifndef DbgSerialBaud
  #define DbgSerialBaud 115200
#endif

#ifndef DbgSerial
  #define DbgSerial SerialP
#endif

#if DbgSerial == SerialP         //0
  #define DbgPort Serial
#elif DbgSerial == SerialP1    //1
  #define DbgPort Serial1
  #define DbgRx 25
  #define DbgTx 26
#elif DbgSerial == SerialP2     //2
  #define DbgPort Serial2
  #define DbgRx 25
  #define DbgTx 26
#elif DbgSerial == BTSerial     //3
  #define DbgPort BTDbg
#endif

#if IsEsp32 > 0 && DbgSerial > 0 // All Esp32 
  #if DbgSerial == BTSerial  
    #include <BluetoothSerial.h> //
    BluetoothSerial DbgPort;
  #else
    #include <HardwareSerial.h> //serial 1 or 2
    HardwareSerial DbgPort(1);
  #endif
#endif


bool SerialInit(unsigned long Brate)
{
  if (!SerialOn)
  { 
    #if IsEsp32 > 0 && DbgSerial > SerialP  
      #if DbgSerial == BTSerial 
        DbgPort.begin("BT DbgPort");
      #else
        DbgPort.begin(Brate,SERIAL_8N1,DbgRx,DbgTx);
      #endif
    #else
      DbgPort.begin(Brate);
    #endif

    #if (BoardType != Uno) && (DbgSerial < BTSerial)
      while (!DbgPort);
    #else delay(500);
    #endif

    #if IsEsp8266 > 0
      DbgPort.println();
      // pinMode(15,OUTPUT);
      // digitalWrite(15,LOW);
      // pinMode(15,INPUT);
     #endif
     DbgPort.print(F("Serial @"));
     DbgPort.println(Brate);
     SerialOn = true;
  }
  return true;
}
   
template< typename S, typename T, typename U > void msg( S Txt, T scratch, U crlf );

template< typename S, typename T, typename U > void msg( S Txt, T scratch, U crlf )
{
     
    #if defined debugpin
      bool debugon = digitalRead(debugpin);
    #else 
      bool debugon = true;
    #endif     
    if (debugon && !SerialOn) 
    {
      SerialOn = SerialInit(DbgSerialBaud); 
    }
  
    if (debugon && SerialOn) 
    {
 //     #if IsEsp8266 > 0
 //       DbgPort.print(F("Freemem: "));
 //       DbgPort.println(ESP.getFreeHeap());
 //     #endif
      DbgPort.print(Txt);
      DbgPort.print(F(":"));
      DbgPort.print(scratch);
      if (crlf) DbgPort.println();
    }  
    else if (SerialOn)
      {
        DbgPort.end();
        SerialOn = false;
      }
}

template< typename S, typename T, typename U > void Dbg( S Txt, T scratch, U crlf );

template< typename S, typename T, typename U > void Dbg( S Txt, T scratch, U crlf )
{
  #if defined debug     
    msg(Txt,scratch,crlf);
  #endif
}
#endif