#ifndef ZenSetPins
  #define ZenSetPins

  #define pp_NoPin 99

  #if IsEsp8266 > 0  //All Esp8266
    #define pp_tx    1
    #define pp_rx    3
    #define pp_flash 0
    #define pp_led   2  // onboard led
    #define pp_wake 16  //16
    #define pp_clk  14
    #define pp_miso 12
    #define pp_mosi 13
    #define pp_cs   15
    #if BoardType == D1MiniPro || BoardType == D1Mini
      #ifdef debug
        #if DbgSerial != SerialP1 // need pin as debugserial output
          #define RcPin D2
        #endif
      #else
        #define RcPin D2
      #endif
    #else //8266-12
      #ifdef debug 
        #if DbgSerial != SerialP1 // need pin as debugserial output
          #define RcPin pp_rx //3
        #endif
      #else
        #define RcPin pp_rx //3
      #endif
    #endif
  #elif IsEsp32 > 0
    #define RcPin 4 
    #define pp_wake  pp_NoPin
    #define pp_flash pp_NoPin
    #define pp_tx    1 
    #define pp_rx    3 
    #define pp_led   2 // on board led
    #define pp_cs    5
    #define pp_clk   18
    #define pp_miso  19
    #define pp_mosi  23
  #elif BoardType == Mkr1000 
    #define RcPin    0  // use interrupt 0 =>  pin 3 on leo pin 0 on teensy
    #define pp_led   6 // on board led
    #define pp_miso  10
    #define pp_mosi  8
    #define pp_clk   9
    #define pp_rx    13
    #define pp_tx    14
  #elif IsTeensy > 0  
    //T3Breakout must disable esp8266 with jumper to use 0 
    //else use 2 and move IsOpen to 10
    #define RcPin A1
  #elif BoardType == Leo
    #define RcPin 3  // use interrupt 0 =>  pin 3 on leo pin 0 on teensy
  #endif
#endif