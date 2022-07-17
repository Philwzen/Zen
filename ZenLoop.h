#ifndef ZenLoop
  #define ZenLoop
  //Stuff that must be done in loop

  #if defined RcPin 
    #if defined RcProc
      RcProc();
    #endif
  #endif

  #ifndef NoDog
    KickDog();
  #endif

  #ifndef NoOta
    OtaCheck();  //preprocessors!!
  #endif

  #ifndef NoMdns
 //     MDNS.update();
  #endif

  #if FtpType > None
     FtpSrv.handleFTP();   
  #endif

  #ifndef NoHttp
     WebSrv.handleClient();
  #endif

  #if defined TimeAlarms_h
    Alarm.delay(0);
  #endif
#endif