#ifndef ZenSetBoard
#define ZenSetBoard

#ifndef BoardType // only set as default 
  #if defined ARDUINO_AVR_UNO  
    #define BoardType   Uno
  #elif defined ARDUINO_AVR_LEONARDO 
    #define BoardType   Leo
  #elif defined ARDUINO_AVR_FIO 
    #define BoardType   AirBoard
  #elif defined ARDUINO_AVR_PRO  
    #define BoardType   MiniPro
  #elif defined ARDUINO_ESP32_DEV  
    #define BoardType   Esp32
  #elif defined ARDUINO_ESP32_PICO  
    #define BoardType   TinyPico
  #elif defined ARDUINO_ESP8266_GENERIC  
    #define BoardType Esp8266
  #elif defined ARDUINO_ESP8266_NODEMCU  
    #define BoardType   Esp826607
  #elif defined ARDUINO_ESP8266_ESP12  
    #define BoardType   Esp826612
  #elif defined ARDUINO_ESP8266_WEMOS_D1MINI  
    #define BoardType   D1Mini
  #elif defined ARDUINO_ESP8266_WEMOS_D1MINIPRO  
    #define BoardType   D1MiniPro
  #elif defined ARDUINO_SAMD_MKR1000  
    #define BoardType   Mkr1000
  #elif defined __IMXRT1062__  
    #define BoardType   Teensy4
  #elif defined __MK20DX256__  
    #define BoardType   Teensy32
  #endif
#endif
  #if BoardType == Uno
    #define BoardName  "Uno"
    #define BoardClass "Avr"
  #elif BoardType == Leo
    #define BoardName  "Leo"
    #define BoardClass "Avr"
  #elif BoardType == AirBoard
    #define BoardClass "AirBoard"
    #define BoardName  "Avr"
  #elif BoardType == MiniPro
    #define BoardClass "MiniPro"
    #define BoardName  "Avr"
  #elif BoardType == Esp32
    #define BoardClass "Esp32"
    #define BoardName  "Esp32"
  #elif BoardType == TinyPico
    #define BoardClass "Esp32"
    #define BoardName  "TinyPico"
  #elif BoardType == Esp8266
    #define BoardClass "Esp8266"
    #define BoardName "Esp8266"
  #elif BoardType == Esp826607
    #define BoardClass "Esp8266"
    #define BoardName  "Esp826607"
  #elif BoardType == Esp826612
    #define BoardClass "Esp8266"
    #define BoardName  "Esp826612"
  #elif BoardType == D1Mini
    #define BoardName  "D1Mini"
    #define BoardClass "Esp8266"
  #elif BoardType == D1MiniPro
    #define BoardClass "Esp8266"
    #define BoardName  "D1MiniPro"  
  #elif BoardType == Mkr1000
    #define BoardClass "Samd"
    #define BoardName  "Mkr1000"
  #elif BoardType == Teensy4
    #define BoardName  "Teensy4"
    #define BoardClass "Teensy"
  #elif BoardType == Teensy32
    #define BoardName  "Teensy32"
    #define BoardClass "Teensy"
  #endif
#endif