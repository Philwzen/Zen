#ifndef ZenBoards
#define ZenBoards

//#define BoardType 
#define EspBegin     001
#define Esp32Cam     050
#define TinyPico     100
#define Esp32        150
#define DoitDevKitV1 200
#define LilyGo       250
#define Esp8266      300 // < Esp32
#define Esp826601    310
#define Esp826607    320
#define Esp826612    330 //Has SPI 
#define D1Mini       400 //Has SPI
#define D1MiniPro    450 //Has SPI
#define EspEnd       499 // < espressif boards

#define AvrBegin     500
#define Uno          510 
#define MiniPro      400
#define AirBoard     500
#define Leo          600
#define AvrEnd       699 // < Avr boards

#define SamBegin     700
#define Mkr1000      750  
#define SamEnd       799 // < Samd

#define PjcBegin     800
#define Teensy32     850
#define Teensy4      900 
#define PjcEnd       999 // < Teensy

// evaluated as a macro after BoardType has been defined
#define IsEsp     (BoardType >= EspBegin && BoardType <= EspEnd)
#define IsEsp32   (BoardType >= EspBegin && BoardType < Esp8266)
#define IsEsp8266 (BoardType >= Esp8266  && BoardType <= EspEnd)
#define IsAvr     (BoardType >= AvrBegin && BoardType <= AvrEnd)
#define IsSamd    (BoardType >= SamBegin && BoardType <= SamEnd)
#define IsTeensy  (BoardType >= PjcBegin && BoardType <= PjcEnd)
#define IsSimcom  (GprsPresent >= SimcomBegin && GprsPresent <= SimcomEnd)

//WiFiType stuff
//#define WiFiType Esp8266
#define None	      0
#define NativeEsp     100
#define AirLift       200
#define EspNow        250
#define AtEsp8266     400
#define SpiEsp8266    500
#define MkrWiFi101    600 
#define MkrNina       650  
#define Cc3000        700
#define Rj45          900  // cable ethernet
#define Default       999  // will then autoset 

//File Systems
//#define FileSys Sdcard
// None 0 
#define Eeprom 1
#define Spiffs 2
#define Sdcard 3

// GprsPresent
//None 0
#define GprsSlave   100 // using sim800 ch
#define GprsMaster  200
#define SimcomBegin 799
#define Sim800      800
#define Sim900      900
#define SimcomEnd   901

// FtpType
// None
#define NatveFtp        999
#define Esp8266Ftp 	100 
#define Esp32Ftp	200 
#define ZenFtp		900

//Station Type for EspNow
//#define StationType Transmitter
#define Transmitter 100
#define Receiver    200

#endif