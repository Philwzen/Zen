#ifndef ZenWSetWiFi
#define ZenSetWiFi

#ifndef WiFiType
  #define WiFiType Default
#endif

#if WiFiType == Default // only set as default 
  #if IsEsp > 0 
  //Esp32 TinyPico Esp8266 Esp826607 Esp826612 D1Mini D1MiniPro Mkr1000
    #define WiFiType NativeEsp
  #elif IsSamd > 0
    #define WiFiType MkrWiFi101 //MkrWiFi101
  #else // No built in wifi 
        //Uno Leo AirBoard MiniPro Teensy4 Teensy32
    #define WiFiType None
  #endif
#endif
#endif