#ifndef Zen
#define Zen
#define Srv IAm

struct SysVar{
  float  Version;      //overridable via prepro
  bool   UseSecurity;  //overridable via prepro
  String AdminList;    //overridable via prepro
  bool   LogAll;       //overridable via prepro
  int    T0Interval;   //overridable via prepro
  #if IsAvr > 0
    long ReBootInterval;//overridable via prepro
  #else
    int  ReBootInterval; //overridable via prepro
  #endif
  int    WiFiTries;      //overridable via prepro
  int    NetCheckInt;    //overridable via prepro
};
SysVar Policy;

#include <ZenPreProDefs.h>

bool FSUp = false;

#include <ZenBareBones.h>  // essential stuff

  #if WiFiType == EspNow
    #include <ZenWiFi.h>
  #elif WiFiType > None  //WiFi Stuff
    char* ssid[]     = {AccessPoints};
    char* pass[] = {PassWords};
    char* SrvName = Srv;
    bool SetupWifi(char* ssid[],char* pass[],int tries);
    #include <ZenWiFi.h>
    #include <ZenPBox.h>  
    //Blynk Stuff
    #if defined BlynkAuthId
      #include <ZenBlynk.h>
    #endif
    #ifndef NoHttpUpdates
      #include <ZenUpdsrv.h>
    #endif
    #ifndef NoOta
       #include <ZenOta.h>
    #endif
  #endif

#ifndef BareBones
  #if IsSamd > 0 // Mkr1000 Analogread Correction 
    #include "SAMD_AnalogCorrection.h"
  #endif

  #include <ZenTime.h>

  /*** stuff to maybe be removed  ***/
  #if IsTeensy > 0  
    #include <ZFastAnalog.h>
  #endif
#endif // end not  barebones

void ZenInit(long Baud, char * ProgTitle)
{  
  Dbg(BoardClass,BoardName,1);
  Dbg(F("Initialising"), ProgTitle, 0);

  //Flash Status Leds
//  ShowStatus(0,1,0);
//  delay(500);	
//  ShowStatus(0,0,0);
  #if defined debug
    SerialOn = SerialInit(Baud);
  #endif 
  SetWDog(0,200);

  #if FileSys >  None
    ShowStatus(GetLed(FsErrorLed),1,0);
    FSUp = InitFileSys();
    if (FSUp) 
    {
      Dbg(F(" On File System"),FsName,1); 
      ShowStatus(GetLed(FsErrorLed),0,0);
    }
  #endif    
  Policy = LoadPolicies(PolicyFile);
  Dbg(F("  Version"),Policy.Version,1);

  #if defined PreWiFi
    PreWiFi();
  #endif
  #if WiFiType > None && TryAps >= 0 
    int cnt = 0;
    ShowStatus(GetLed(WiFiErrorLed),1,0);
    #if WifiType == EspNow
      SetUpEspNow();
    #else
      while (cnt < Policy.WiFiTries)
      {
        if (SetupWifi(ssid,pass,Policy.WiFiTries))
        {
          #ifndef NoHttpUpdates
            CheckHttpUpdateAvailable();
          #endif
          #ifndef NoOta  // BoardType != Mkr1000   
            SetupOTA(SrvName);
          #endif
          break;
        } 
        else 
        {
          cnt++;
          Dbg(F("NetWork Retry"),cnt,1);
        }
        if (cnt >= Policy.WiFiTries) FatalError("Giving up No NetWork!!",WiFiErrorLed);
      } 
    #endif 
    ShowStatus(GetLed(WiFiErrorLed),0,0);
  #endif
  #if IsSamd > 0
    #if MkrNum > 0
      analogReadCorrection(MkrAC1,MkrAC2);
    #endif
  #endif

  #if defined PostWiFi
     if (!PostWiFi) FatalError("PostWiFi Failed",RedLed);
  #endif
  #if defined RcPin
    #if defined RcProc
      InitRc(RcPin);  // could do as postwifiin z433.h
    #endif
  #endif 
  #ifndef BareBones 
    #if defined UseDdns
      SetDdns();
    #endif
    #if defined ZenTime
      SetDateTime();
    #endif
    SetUpTimers();  
    #ifdef BLYNK_FIRMWARE_VERSION
      BlynkEdgent.begin();
    #endif
  #else
    Dbg(F("BareBones"),F("Only"),1);
  //  Dbg(F("Initialising"), ProgTitle, 1); 
  #endif
  #if BoardType == TinyPico
    tp.DotStar_CycleColor(25);
  #else
    ShowStatus(GetLed(AliveLed),1,0);
  #endif
}

#endif