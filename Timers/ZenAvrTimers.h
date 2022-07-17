#ifndef ZAvrTimers
#define ZAvrtimers
// Timers
// Timer Options 

  #define HWSec 1000
//  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
  #ifdef PPT0Proc  
    #if PPT0Interval > 0
//      hw_timer_t * T0Timer = NULL;  // max 4 timers 0,1,2,3
      void IRAM_ATTR PPT0Proc();
    #endif

  #if defined PPT1Proc
    #if PPT1Interval > 0
//      hw_timer_t * T1Timer = NULL;  // max 4 timers 0,1,2,3
      void IRAM_ATTR PPT1Proc();
    #endif
  #if defined PPT2Proc
    #if PPT2Interval > 0
//      hw_timer_t * T2Timer = NULL;  // max 4 timers 0,1,2,3
      void IRAM_ATTR PPT2Proc();
    #endif
  #if defined PPT3Proc
    #if PPT3Interval > 0
//      hw_timer_t * T3Timer = NULL;  // max 4 timers 0,1,2,3
      void IRAM_ATTR PPT3Proc();
    #endif
  #endif

void SetUpHwareTimers()
{
    // max 4 timers 0,1,2
    // interval in microsecs 1 sec = 1 000 000
    #ifdef PPT0Proc   
      T0Timer = timerBegin(0, 80, true);
      timerAttachInterrupt(T0Timer, &PPT0Proc, true);
      timerAlarmWrite(T0Timer, Policy.T0Interval * HWSec, true);
      timerAlarmEnable(T0Timer);
    #endif
    #if defined PPT1Proc
      T1Timer = timerBegin(1, 80, true);
      timerAttachInterrupt(T1Timer, &T1Proc, true);
      timerAlarmWrite(T1Timer, PPT1Interval * HWSec, true);
      timerAlarmEnable(T1Timer);
    #endif
    #if defined PPT2Proc
      T2Timer = timerBegin(2, 80, true);
      timerAttachInterrupt(T2Timer, &T2Proc, true);
      timerAlarmWrite(T2Timer, PPT2Interval * HWSec, true);
      timerAlarmEnable(T2Timer);
    #endif
    #if defined PPT3Proc
      T3Timer = timerBegin(3, 80, true);
      timerAttachInterrupt(T3Timer, &T3Proc, true);
      timerAlarmWrite(T3Timer, PPT3Interval * HWSec, true);
      timerAlarmEnable(T3Timer);
    #endif
    // Stop and free a timer by doing
    // timerEnd(xxxTimer);
    // xxxTimer = NULL;
}  
#endif