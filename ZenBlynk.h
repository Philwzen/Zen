#ifndef ZenBlynk
  #define ZenBlynk

  #ifndef BlynkAuthId
    #define BlynkAuthId "Ei4kFJG5k63O42ljgALlXqtWtqAQ0sMd"
  #endif 
  #ifndef BlynkTemplate 
    #define BlynkTemplate "TMPLR_TbeUhk"
  #endif 
  #ifndef BlynkDevice 
    #define BlynkDevice "MkrController"
  #endif 

  //  char auth[] = "Ei4kFJG5k63O42ljgALlXqtWtqAQ0sMd";
  #define BLYNK_TEMPLATE_ID BlynkTemplate
  #define BLYNK_DEVICE_NAME BlynkDevice

  #if WiFiType == NativeEsp
    #if IsEsp32 > 0
      #include <BlynkSimpleEsp32.h>
    #else   //IsEsp8266
      #include <BlynkSimpleEsp8266.h>
    #endif
  #elif WiFiType == AirLift    
    #include <BlynkSimpleWiFiNINA.h>
  #elif WiFiType == AtEsp8266   
  #elif WiFiType == SpiEsp8266    
  #elif WiFiType == MkrWiFi101     
    #include <BlynkSimpleMKR1000.h>
  #elif WiFiType == MkrNina         
    #include <BlynkSimpleWiFiNINA.h>
  #elif WiFiType == Cc3000        
    #include <BlynkSimpleCC3000.h>
  #elif WiFiType == Rj45 // cable ethernet
    #include <BlynkSimpleEthernet.h>
  #endif
  #ifdef BLYNK_FIRMWARE_VERSION
    #include <BlynkEdgent/BlynkEdgent.h>
  #endif
#endif  

 
 