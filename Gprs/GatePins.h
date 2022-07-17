#ifndef GatePins
#define GatePins

#if IsEsp32 > 0 // esp32
    /* Status input from gate switches*/
    #define IsOpen   21
    #define IsClosed 22
    /* Relay Pin Numbers */
    #define GateOpen  32
    #define GateClose 33
#elif IsEsp8266 > 0
  #if BoardType == D1MiniPro || BoardType == D1Mini
    #define IsOpen    A0  
    #define IsClosed  D0
    #define GateOpen  D3  
    #define GateClose D4  
  #else //8266-12
    #define IsOpen    pp_miso //12
    #define IsClosed  pp_clk  //14
    #define GateOpen  5
    #define GateClose 4
  #endif
#elif BoardType == Mkr1000
  #define IsOpen    2
  #define IsClosed  3
  #define GateOpen  5
  #define GateClose 6
#elif IsTeensy > 0  
  #define IsOpen    2
  #define IsClosed  3
  #define GateOpen  5
  #define GateClose 6
#elif BoardType == Leo
  #define IsOpen    2
  #define IsClosed  4
  #define GateOpen  5
  #define GateClose 6
#endif

#endif

