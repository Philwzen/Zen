#ifndef ZEspTimers
#define ZEsptimers
// Timers
// Timer Options 

#if IsEsp8266 > 0
  void ICACHE_RAM_ATTR T0Proc();
#else
  #if defined PPT0Proc
    void IRAM_ATTR PPT0Proc();
  #endif
  #if defined PPT1Proc
    void IRAM_ATTR PPT1Proc();
  #endif
  #if defined PPT2Proc
    void IRAM_ATTR PPT2Proc();
  #endif
  #if defined PPT3Proc
    void IRAM_ATTR PPT3Proc();
  #endif
#endif

void SetUpHwareTimers()
{
#if IsEsp8266 > 0
  #define HWSec 312500  //312500 @3.2tick microsec esp8266
  #if defined PPT0Proc
    timer1_attachInterrupt(PPT0Proc);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE); // 1 tick = 3.2us
    timer1_write(Policy.T0Interval * HWSec); 
  #endif 
#else
  // interval in microsecs 1 sec = 1 000 000
  #define HWSec 1000000    // microsec esp32
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
  #if defined PPT0Proc
     hw_timer_t * T0Timer = NULL;  // max 4 timers 0,1,2,3
     T0Timer = timerBegin(0, 80, true);
     timerAttachInterrupt(T0Timer, &PPT0Proc, true);
     timerAlarmWrite(T0Timer, Policy.T0Interval * HWSec, true);
     timerAlarmEnable(T0Timer);
  #endif 

  #if defined PPT1Proc
     hw_timer_t * T1Timer = NULL;  // max 4 timers 0,1,2,3
     T1Timer = timerBegin(1, 80, true);
     timerAttachInterrupt(T1Timer, &PPT1Proc, true);
     timerAlarmWrite(T1Timer, PPT1Interval * HWSec, true);
     timerAlarmEnable(T1Timer);
  #endif 

  #if defined PPT2Proc
    hw_timer_t * T2Timer = NULL;  // max 4 timers 0,1,2,3
    T2Timer = timerBegin(2, 80, true);
    timerAttachInterrupt(T2Timer, &PPT2Proc, true);
    timerAlarmWrite(T2Timer, PPT2nterval * HWSec, true);
    timerAlarmEnable(T2Timer);
  #endif 

  #if defined PPT3Proc
    hw_timer_t * T3Timer = NULL;  // max 4 timers 0,1,2,3
    T3Timer = timerBegin(3, 80, true);
    timerAttachInterrupt(T3Timer, &PPT3Proc, true);
    timerAlarmWrite(T3Timer, PPT3Interval * HWSec, true);
    timerAlarmEnable(T3Timer);
  #endif 
#endif
    // Stop and free a timer by doing
    // timerEnd(xxxTimer);
    // xxxTimer = NULL;
}  

void EspWdt(long interval) // millisecs
{ 
  #if IsEsp8266 > 0
   if (interval == 0)
   {
     ESP.wdtDisable();
     *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
   }
   else 
   {  //WDTO_500MS  = 500, //!< WDTO_500MS
    ESP.wdtEnable(interval); //   WDTO_1S xxx ms
    *((volatile uint32_t*) 0x60000900) |= 1; // Hardware WDT ON
   }
  #endif
}
#endif