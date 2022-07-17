#ifndef ZenWSetFileSys
#define ZenSetSetFileSys

#ifndef FileSys
  #define FileSys Default
#endif

#if FileSys == Default // only set as default 
  #if IsEsp > 0 
  //Esp32 TinyPico Esp8266 Esp826607 Esp826612 D1Mini D1MiniPro Mkr1000
    #define FileSys Spiffs
  #else//Uno Leo AirBoard MiniPro Teensy4 Teensy32
    #define FileSys None
  #endif
#endif
#endif