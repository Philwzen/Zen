#ifndef Z433
#define Z433

#if defined RcProc

  bool ReceivedRc(unsigned long val);

  #include <RCSwitch.h>
  RCSwitch RemoteRc = RCSwitch();

  String Get433Id(unsigned long val)
  {
    char rv[10];  // unsigned long decimal
    snprintf(rv, 10, "%lu", val);
    return ToString(rv);
  }

  void InitRc(int OnPin)
  {
    Dbg("Init 443 on Pin",OnPin,1);
    #if IsEsp> 0
      RemoteRc.enableReceive(digitalPinToInterrupt(OnPin));
    #else
      RemoteRc.enableReceive(OnPin); 
    #endif
  }
#else
  void InitRc(int OnPin);
  String Get433Id(unsigned long val);
#endif

#endif

