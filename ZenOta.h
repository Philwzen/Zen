#ifndef ZenOta
#define ZenOta

//void SetupOTA(char* HostName);
#define ArdOTA ArduinoOTA
#define MkrOTA WiFiOTA

#if IsSamd > 0     //BoardType == Mkr1000
  #include <WiFi101OTA.h>
  #define OtaType WiFiOTA
  #define OtaCheck WiFiOTA.poll
#elif IsEsp > 0
  #include <ArduinoOTA.h>
  #define OtaType ArduinoOTA
  #define OtaCheck ArduinoOTA.handle
#else
  #define OtaType None
  #define OtaCheck None
#endif


//#ifndef OTAPort
//  #if BoardType < EspEnd
//    #if Boardtype < Esp8266
//     #define OTAPort 3232
//    #else
//      #define OTAPort 8266
//    #endif
//  #else 
//    #define OTAPort 5353
//  #endif
//#endif  

void SetupOTA(char* HostName)
{
  char hostname[20];
  // lowercase else dont seem to work!!
  String Hname(HostName);
  Hname.toLowerCase();
  strcpy(hostname, Hname.c_str());
  Dbg(F("OTAHostName"),hostname,1);

  #if IsEsp > 0   //OtaType == ArduinoOTA
    #if defined OTAPort  // prob ok to just default it
      OtaType.setPort(OTAPort);
    #endif

    OtaType.setHostname(hostname); // default esp8266-[ChipID]
    // OtaType.setPassword((const char *)"123"); // defaultno authentication
 
    OtaType.onStart([]() 
    {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) 
      {
        type = "sketch";
      }  
      else 
      { // U_FS
        type = "filesystem";
      }   
      Dbg(F("OTA"),F("Started"),1);
    });
    OtaType.onEnd([]() 
    {
      Dbg(F("OTA"),F("Ended"),1);
    });
    OtaType.onProgress([](uint done,uint tot) 
    {
      Dbg(F("OTA: %"), (done / (tot / 100)),1);
    });
    OtaType.onError([](ota_error_t error) 
    {
      Dbg("Error", error,0);
      if (error == OTA_AUTH_ERROR) Dbg(F("Auth"),F("Failed"),1);
      else if (error == OTA_BEGIN_ERROR) Dbg(F("Begin"),F("Failed"),1);
       else if (error == OTA_CONNECT_ERROR) Dbg(F("Connect"),F("Failed"),1);
        else if (error == OTA_RECEIVE_ERROR) Dbg(F("Receive"),F("Failed"),1);
         else if (error == OTA_END_ERROR) Dbg(F("End"),F("Failed"),1);
    });

    OtaType.begin();
  #elif IsSamd > 0   //OtaType == WiFiOTA
    OtaType.begin(hostname, "ota", InternalStorage);
  #else // do nothing
    Dbg("No ota",OtaType,1);
  #endif
  Dbg(F("OTA"),F("Ready"),1);
}
#endif