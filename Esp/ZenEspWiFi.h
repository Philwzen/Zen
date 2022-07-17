#ifndef ZEspWiFi
#define ZEspWiFi

#if WiFiType == NativeEsp
  #if IsEsp32 > 0
    #include <WiFi.h>     //wifinina
    #ifndef NoMdns
      #include <ESPmDNS.h>
    #endif
    #ifndef NoHttp || FtpType > None
      #include <WebServer.h>
    #endif
    #define WiFiServer WebServer
  #else      // IsEsp8266
    #include <ESP8266WiFi.h>   
    #ifndef NoMdns
      #include <ESP8266mDNS.h>
    #endif
    #ifndef NoHttp || FtpType > None
      #include <ESP8266WebServer.h>
    #endif
    #define WiFiServer ESP8266WebServer
  #endif
  #include <WiFiUdp.h>
  #include <WiFiClient.h>
#elif WiFiType == SpiEsp8266 // Spi based ok
  // missing in WIFI mode localip softAPConfig macAddr 
  // teensy/t4/ildaespspitran
  #define NoNetFunctions
  #define NoWebFunctions

  #ifndef WiFiPin
    #define WiFiPin 10
  #endif
  #include <SPI.h>    
  #include <WiFiSpi.h>
  #include <WiFiSpiUdp.h> 

  #define WiFiUDP    WiFiSpiUdp
  #define WiFiClient WiFiSpiClient
  #define WiFiServer WiFiSpiServer
  //#define WiFi. WiFiSpi.  change in WiFiSpi.h extern 
   
#elif WiFiType == AtEsp8266  // AT Commands yuck
 // #if BoardType >= PjcBegin && BoardType <= PjcEnd
  #if defined HWSerial || (BoardType >= PjcBegin && BoardType <= PjcEnd)
    #define EspSerial Serial1
  #else
    #include <SoftwareSerial.h>
    #ifndef WiFiSerialRX
      #define WiFiSerialRX 10
    #endif
    #ifndef WiFiSerialTX
      #define WiFiSerialTX 11
    #endif
    SoftwareSerial WiFiSerial(WiFiSerialRX, WiFiSerialTX);
  #endif
  #ifndef WiFiBaud
    #define WiFiBaud 115200 // ?? to fast ??
  #endif
  #include <WiFiEsp.h>
  #include <WiFiEspUdp.h>
  #define WiFiUDP WiFiEspUDP
  #include <WiFiEspClient.h>
  #define WiFiClient WiFiEspClient
  #include <WiFiEspServer.h>
  #define WiFiServer WiFiEspServer 

//  need to test this one 
   #ifndef NoHttp || FtpType > None
     #include <WiFiWebServer.h>
   #endif

//  #ifndef NoHttp
//    #define WiFiServer ESP8266_AT_WebServer
//    #define _ESP_AT_LOGLEVEL_  0 // 0:DISABLED 1:ERROR 2:WARN 3:INFO 4:DEBUG
//    #define USE_NEW_WEBSERVER_VERSION 1
//    #define DEBUG_ESP8266_AT_WEBSERVER_PORT Serial
//    #include <ESP8266_AT_WebServer.h>
//  #endif

#elif WiFiType == AirLift    // esp32 spi ok
  // teensy/t4/airlifttest
  // orange +v yellow mosi green 0v
  // blue   na prple  clk  grey  miso

  #include <SPI.h>

  //SPIWIFI      The SPI Port
  //SPIWIFI_SS   Slave Select Pin
  //SPIWIFI_ACK  Busy/Ready Pin 
  //ESP32_RESETN Reset Pin
  //SP32_GPIO0   -1 ignore
  // miso 12   mosi 11   sck 13   all pins on t4

  #define SPIWIFI       SPI  // The SPI port
  #ifndef SPIWIFI_SS
    #define SPIWIFI_SS       21 //Grey   A7 // normally 10 Chip select pin
  #endif
  #ifndef SPIWIFI_ACK
    #define SPIWIFI_ACK      22 //Purple A8 // 9 a.k.a BUSY or READY pin  #endif
  #endif
  #ifndef ESP32_RESETN
    #define ESP32_RESETN     23  //Blue   A9 // 6 Reset pin
  #endif
  #define ESP32_GPIO0    -1
  #define WiFiServer WiFiWebServer

  #include <WiFiNINA.h>
  #include <WiFiUdp.h>
  #include <WiFiClient.h>

  #ifndef NoHttp || FtpType > None
     #include <WiFiWebServer.h>
  #endif 
#elif WiFiType == EspNow     // espnow esp native only
  #define WiFiServer NoServer
  #if IsEsp32 > 0
    #include <esp_now.h>
    #include <WiFi.h>
  #else  // IsEsp8266 > 0
    #include <ESP8266WiFi.h>
    extern "C" 
    {
      #include <espnow.h>
      #include <user_interface.h>
    }
  #endif
  #include <Esp/ZenEspNow.h>
#endif

#endif