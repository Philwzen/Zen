#ifndef SimcomPins
#define SimComPins

/* 
#define PowerStatusPin 23 // LilyGo 23      GprsMaster na   Gprsslave 9
#define OnOffPin 4        // LilyGo 4       GprsMaster na   Gprsslave 12
#define ResetPin 5        // LilyGo 5       GprsMaster na   Gprsslave na
#define LED_GPIO 13     
#define LED_ON   HIGH   
#define LED_OFF  LOW    
#define GprsTx   27       // LilyGo 27      GprsMaster 25   Gprsslave 8
#define GprsRx   26       // LilyGo 26      GprsMaster 26   Gprsslave 7
#define GprsBaud 9600     // LilyGo 115200  GprsMaster 9600 Gprsslave 9600
*/

#if GprsPresent == GprsMaster
 #if IsEsp32 > 0 // All Esp32 
    #define PowerStatusPin 27
    #define OnOffPin pp_NoPin
    #define ResetPin pp_NoPin
    #define GprsRx 25
    #define GprsTx 26
    #include <HardwareSerial.h>
    #define SimcomSerial Serial1
  #elif IsEsp8266 > 0 // All Esp8266 types with Spi
    #define OnOffPin pp_NoPin
    #define ResetPin pp_NoPin
    #include <SoftwareSerial.h>
    #if BoardType == D1MiniPro || BoardType == D1Mini
      #define PowerStatusPin D1
      #define GprsTx pp_tx
      #define GprsRx pp_rx
    #else
      #define PowerStatusPin 14
      #define GprsTx 13
      #define GprsRx 15 
    #endif
    SoftwareSerial SimcomSerial(GprsRx,GprsTx);
  #elif IsSamd > 0
    #define PowerStatusPin A2
    #define OnOffPin 4
    #define ResetPin 5
    #define GprsTx pp_tx
    #define GprsRx pp_rx
    #define SimcomSerial Serial1
  #elif IsTeensy > 0
    //#define PowerStatusPin 23
    //#define OnOffPin pp_NoPin
    //#define ResetPin pp_NoPin
    #define GprsTx 7
    #define GprsRx 8
    #define SimcomSerial Serial3
  #endif
#elif GprsPresent == GprsSlave // only leo
  //#if BoardType == Leo
    #define PowerStatusPin 12
    #define OnOffPin 9
    #define ResetPin 5
    #include <SoftwareSerial.h>
    #define GprsTx 7
    #define GprsRx 8
    SoftwareSerial SimcomSerial(GprsRx,GprsTx);
  //#elif //nothing
  //#ndif
#elif GprsPresent == Sim800  // leo or lilygo 
  #if BoardType == LilyGo
    #define PowerStatusPin 23
    #define OnOffPin 4
    #define ResetPin 5
    #define GprsRx 26
    #define GprsTx 25
    #define SimcomSerial Serial1
  #elif BoardType == Leo 
    #define PowerStatusPin 12
    #define OnOffPin 9
    #define ResetPin 5  
    #define GprsTx 7 //0
    #define GprsRx 8 //1
    #define SimcomSerial Serial1
  #endif		 
#endif
#endif