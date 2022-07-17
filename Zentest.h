#ifndef P1 
 #define P1 "undefined" 
#endif

  #ifndef NoOta
    ArduinoOTA.handle(); //ota check
  #endif
  #ifndef NoMdns
      MDNS.update();
  #endif
  #if FtpType > 0
     FtpSrv.handleFTP();
  #endif
  #ifndef NoHttp
     WebSrv.handleClient();
  #endif
 
Dbg("from zentest",P1,1);


#ifdef P1
  #undef P1
#endif