// PreProcessor defaults
//dbg ports
#define SerialP 0 
#define SerialP1 1
#define SerialP2 2
#define BTSerial 3

//#define Core ./
//#define Libs Libraries/


#include <ZenSetBoard.h>
#include <ZenSetWiFi.h>
#include <ZenSetFileSys.h>
#include <ZenSetPins.h>
#include <ZenSetLeds.h>

/** Mandatory definitions **/
#define NoHttpUpdates // dont load httpupdate stuff
#define NoMdns        // dont load Mdns stuff

#ifndef DelimOne
  #define DelimOne '^'
#endif
#ifndef DelimTwo
  #define DelimTwo '#'
#endif
#ifndef WiFiType
  #define WifiType None
#endif
#ifndef FileSys
  #define FileSys  None
#endif
#ifndef GprsPresent
  #define GprsPresent None
#endif
#ifndef IAm
  #define IAm "IAm Undefined"
#endif  
#ifndef Srv
  #define Srv IAm
#endif
#ifndef RebootOnFatal
  #define RebootOnFatal 3  // reboot after x sec on FatalError
#endif
#ifndef AutoAp
  #define AutoAp -1
#endif
#ifndef DbgSerialBaud 
   #define DbgSerialBaud 115200
#endif
/*********** Sys Defaults ***********/
/** Policies and SysVars **/
#ifndef PolicyFile
  #define PolicyFile "HRPolicy.txt"
#endif
#ifndef SysVarFile 
  #define SysVarFile "SysVar.txt"
#endif
#ifndef UserFile
  #define UserFile "UsrLst.txt"
#endif
#ifndef PPVersion  
  #define PPVersion 9.99
#endif
#ifndef PPAdminList    
  #define PPAdminList "1320phil,1320phili,9373718,+6799373718"
#endif
#ifndef PPLogAll        
  #define PPLogAll true
#endif
#ifndef PPReBootInterval     
  #define PPReBootInterval 86400
#endif
#ifndef PPT0Interval   
  #define PPT0Interval 5
#endif
#ifndef PPT0Proc     
  #define PPT0Proc ImAlive
#endif
#ifndef PPWiFiTries     
  #define PPWiFiTries 3
#endif
#ifndef PPNetCheckInt   
  #define PPNetCheckInt 10
#endif
/**********/
/** Esp Only */
#if IsEsp > 0
 #if IsEsp8266
   #define NoDog
 #endif
 #ifndef WiFiSetFlash 
   #define  WiFiSetFlash  0
 #endif
#endif
/**********/
/** Just the minimum **/ 
#ifdef BareBones
  #define NoDog
  #define NoHWTimers        // really jjust for avr
  #define WiFiSetFlash  0   // WiFiFlashSettings()
#endif
/**********/
/** If No Network **/
#if WiFiType == None    // no network so
  #define NoOta         // dont load Ota update stuff
  #define NoMdns        // dont load Mdns stuff
  #define NoHttpUpdates // dont load httpupdate stuff
  #define FtpType  None // dont start ftp server
  #define NoHttp        // dont start http server
  #define AutoAp    -1  // dont start ap if no wifi found
#endif
/*********/
/** Default Ftp **/
#ifndef FtpType || FileSys == None
  #define FtpType None 
#endif

#if FtpType == Default
  #if WiFiType == NativeEsp
    #if IsEsp32 > 0 
      #define FtpType Esp32Ftp     
    #elif IsEsp8266 > 0  
      #define FtpType Esp8266Ftp  
    #else
      #define FtpType NatveFtp  
    #endif
  #endif 
#endif

/**********/
/** Default File System **/
#if FileSys == None
  char FsName[5] =  "None";
#elif FileSys == Spiffs 
  char FsName[7] =  "Spiffs";
#elif FileSys == Eeprom
  char FsName[7] =  "Eeprom";
#elif FileSys == Sdcard
  char FsName[7] =  "Sdcard";
#endif
/**********/
#if IsSamd > 0 // Mkr1000 Analogread Correction 
  #ifndef MkrARes
    #define MkrARes 12
  #endif 
  #ifndef MkrNum
    #define MkrNum 0
  #endif
  #if MkrNum == 1
    #define MkrAC1 8       // boards pecific
    #define MkrAC2 2069    //
  #elif MkrNum == 2
    #define MkrAC1 0       // boards pecific
    #define MkrAC2 2058    //
  #elif MkrNum == 3
    #define MkrAC1 6       // t
    #define MkrAC2 2067    //
  #elif MkrNum == 4
    #define MkrAC1 94      // 
    #define MkrAC2 2099    //
  #endif
#endif
/**********/
/** OtA Programming **/
#if IsEsp == 0  && BoardType != Mkr1000
  #define NoOta      //otadontworkwell
#endif
/**********/
/**********/
/** Time Sync Interval **/ 
#define TimeSyncInterval 3600  // resysnc time every hr resyncs on boot anyway!


/** WiFi Connection **/
#ifndef TryAps    // ap's to try
  #define TryAps 5  // or 1???
#endif   
#ifndef UseNetId
  #if WiFType == MkrWiFi101
    #define UseNetId 4  //-1=autoselect else = pos in Accesspoints
  #else
    #define UseNetId -1  //-1=autoselect else = pos in Accesspoints
  #endif
#endif
#ifndef AccessPoints
  #define AccessPoints "Me","Repeater250","Marvin","Laser","Casablanca"
#endif
#ifndef PassWords
  #define PassWords "","Crasies123","Crasies123","","123Bernard"        
#endif
/**********/

//Version Stuff   NOT USED
#define UpdServer "http://191.168.0.240/" // "Http://EspUdpServer"
#define UpdVerFile Srv "/Version.txt"
#define VerData UpdServer "GetSrvVersion?VersionFile=" UpdVerFile
#define UpdData UpdServer  Srv ".bin"    // url of update binary

/** Web Server **/
#ifndef HttpPort
  #define HttpPort 80
#endif
#ifndef WebRootFunction
  #define WebRootFunction LoadIndexRoot
#endif
#ifndef RootPage
  #define RootPage "index.htm"
#endif
/**********/
