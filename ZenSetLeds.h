#ifndef ZenSetLeds
#define ZenSetLeds
//Status stuff
// To many pins used !!! 
//posistions in array Leds of each pin
#define OffLed      0 
#define RedLed      1  
#define GreenLed    2
#define BlueLed     3 
#define OrangeLed   4 //wifierrorled 
#define VioletLed   5 //fserrorled
#define TurqoiseLed 6 
#define AliveLed    7
#define ResetLed   -1
#define CycleLed   -2
#define DummyLed    AliveLed  // just so anything not defined works.

#if IsEsp > 0
  #if BoardType == TinyPico
    int Leds[] = {0,1,2,3,4,5,6,7};
  #elif bordType == Esp32Cam
    int Leds[] = {0,0,0,0,0,0,0,0};
  #elif IsEsp8266 > 0
    int Leds[] = {0,0,0,0,0,0,0,2};
  #else
    int Leds[] = {0,16,19,18,17,14,15,13}
#endif
  #elif IsAvr > 0
    int Leds[] = {0,0,0,0,0,0,0,13};
  #elif IsSamd > 0
    int Leds[] = {0,0,0,0,0,0,0,13};
  #else
    int Leds[] = {0,16,19,18,17,14,15,13}
  #endif

#endif