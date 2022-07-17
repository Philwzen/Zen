#ifndef ZTeensyTimers
#define ZTeensyTimers

// Timers
// Timer Options 

#ifndef HWSec
  #define HWSec 1000000
#endif

#ifdef PPT0Proc
  IntervalTimer Timer0;
  void PPT0Proc();  
#endif
#ifdef PPT1Proc
  IntervalTimer Timer1;
  void PPT1Proc();  
#endif
#ifdef PPT2Proc
  IntervalTimer Timer2;
  void PPT2Proc(); 
#endif
#ifdef PPT3Proc
  IntervalTimer Timer3;
  void PPT3Proc();  
#endif

  void SetUpHwareTimers() 
  {
    #ifdef PPT0Proc
      Timer0.begin(PPT0Proc, HWSec * Policy.T0Interval);
    #endif
    #ifdef PPT1Proc    
      Timer1.begin(PPT1Proc, HWSec * PPT1Interval);
    #endif
    #ifdef PPT2Proc    
      Timer2.begin(PPT2Proc, HWSec * PPT2Interval);
    #endif
    #ifdef PPT3Proc    
      Timer3.begin(PPT3Proc, HWSec * PPT3Interval);
    #endif
  }

#endif 