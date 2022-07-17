#ifndef SamdWiFi
  #define SamdWiFi

  #define DEBUG_WIFI_WEBSERVER_PORT   Serial
  // Debug Level from 0 to 4
  #define _WIFI_LOGLEVEL_            0//4 
  #define _WIFININA_LOGLEVEL_        0 
  #define WIFI_USE_SAMD true
  #define BOARD_TYPE    "SAMD MKR1000"
  #define BOARD_NAME    BOARD_TYPE
 
  #if WiFiType == MkrWiFi101 // 
    #define USE_WIFI_NINA         false
    #define USE_WIFI101           true
    #include <WiFi101.h>
  #elif WiFiType == MkrNina //  
    #define USE_WIFI_NINA         true
    #define USE_WIFI101           false
    #include <WiFiNINA_Generic.h>
    //#include <WiFiNINA.h>
  #endif
  #ifndef NoHttp 
    #include <WiFiWebServer.h>  
    #define WiFiServer WiFiWebServer
  #endif
  #if FtpType > None
    //#include <FtpServer.h>
    //#include <WiFiWebServer.h>  
  #endif
  #include <WiFiUdp.h> 
#endif