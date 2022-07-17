#ifndef ZenTimers
#define ZenTimers
// Timers
// include the timeralarm library
  #define ASec 1    
  #include <TimeAlarms.h>

#if IsEsp8266 > 0 // its a esp8266
 void ICACHE_RAM_ATTR ImAlive();
#elif IsEsp32    // its a esp32
 void IRAM_ATTR ImAlive();
#else  // everything else ?? teensy
 void ImAlive();
#endif

#ifndef NoHWTimers
  #if IsEsp > 0 //ESP
    #if IsEsp8266 > 0
       #define HWSec 312500  //312500 // @3.2/tick microsec esp8266
    #else
      #define HWSec 1000000  // microsec esp32
    #endif
    #include <Timers/ZenEspTimers.h>
  #elif BoardType >= PjcBegin // Teensy
    #define HWSec 1000000  // microsec
    #include <Timers/ZenTeensyTimers.h>
  #elif IsAvr > 0 
    #include <Timers/ZenAvrTimers.h>
  #elif IsSamd > 0 //etc etc
    #include <Timers/ZenSamdTimers.h> 
  #elif IsTeensy > 0 //etc etc
    #include <Timers/ZenTeensyTimers.h>
  #endif  
#endif
// T0 is alive chack T1 to T3 in Hardware specific includes
// Use by
// #define TxInterval  secs    
// #define TxProc      funcname


  #if defined T4Interval
    AlarmId T4Id;
    void T4Proc();
  #endif
  #if defined T5Interval
    AlarmId T5Id;
    void T5Proc();  
  #endif
  #if defined T6Interval 
    AlarmId T6Id = 0;    
    void T6Proc();  
  #endif
  #if defined T7Interval
    AlarmId T7Id = 0;    
    void T7Proc();  
  #endif              
  #if defined T8Interval   
    AlarmId T8Id = 0;    
    void T8Proc();  
  #endif
  #if defined T9Interval 
    AlarmId T9Id = 0;    
    void T9Proc();  
  #endif
  // can always define more if needed 



  void SetUpTimers()
  {

    #ifndef NoHWTimers
      //  processor specific Ti0 -- T3
      SetUpHwareTimers();
    #endif

    // TimeAlarm 
    #if defined T4Interval
      T4Id = Alarm.timerRepeat(ASec * T4Interval, T4Proc);  
    #endif
    #if defined T5Interval
      T5Id = Alarm.timerRepeat(ASec * T5Interval, T5Proc);
    #endif
    #if defined T6Interval 
      T6Id = Alarm.timerRepeat(ASec * T6Interval, T6Proc);
    #endif
    #if defined T7Interval
      T7Id = Alarm.timerRepeat(ASec * T7Interval, T7Proc);
    #endif
    #if defined T8Interval
      T8Id = Alarm.timerRepeat(ASec * T8Interval, T8Proc);
    #endif
    #if defined T9Interval 
      T9Id = Alarm.timerRepeat(ASec * T9Interval, T9Proc);
    #endif
 }
#endif