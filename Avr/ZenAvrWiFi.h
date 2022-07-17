#ifndef AvrWiFi
   #defineAvrWiFi
   if WiFiType == Cc3000
     // eg leo/cc3kvoltagesender
     Dbg("Using Wifi","CC3000",1);
     #include <Adafruit_CC3000.h>
     // cs,irq,vbat
     Adafruit_CC3000 WiFi = Adafruit_CC3000(10,3,5,SPI_CLOCK_DIVIDER); 
     #define status getStatus
  #endif
#endif