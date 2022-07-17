#ifndef ZSamdTimers
  #define ZSamdtimers
  // HardWare Specific Timer Options 

  #if defined PPT0Proc
    void PPT0Proc();
  #endif
  #if defined PPT1Proc
    void PPT1Proc();
  #endif 
  #if defined PPT2Proc
    void PPT2Proc();
  #endif 
  #if defined PPT3Proc
      void PPT3Procc();
  #endif

void SetUpHwareTimers()
{
  #if defined PPT0Proc
      AlarmId T0Id;
      T0Id = Alarm.timerRepeat(ASec * Policy.T0Interval,PPT0Proc);  
  #endif 
   #if defined PPT1Proc
      AlarmId T1Id;
      T1Id = Alarm.timerRepeat(ASec * PPT1Interval, PPT1Proc);  
  #endif 
  #if defined PPT2Proc
      AlarmId T2Id;
      T2Id = Alarm.timerRepeat(ASec * PPT2Interval, PPT2Proc);
  #endif
  #if defined PPT3Proc 
      AlarmId T3Id;
      T3Id = Alarm.timerRepeat(ASec * PPT3Interval, PPT3Proc);
  #endif
}  

#endif