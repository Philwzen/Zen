#ifndef ZenTime
#define ZenTime

// include the timeralarm library
#define ASec 1   
#define HrSec 3600 
#include <TimeAlarms.h>
   // #define Delay Alarm.delay 
#include <TimeLib.h>
#include <Time.h>
#include <NTPClient.h>

time_t GprsSetTime();

// Time related stuff
#if IsTeensy > 0  
  #if BoardType == Teensy32
    #include <DS1307RTC.h> 
  #endif
  #define RtcPresent
#elif BoardType == TinyPico && defined RtcPresent
  #include <Rtc.h>
  //  mcp7940 RTC;  
#elif BoardType == Mkr1000
  //#include <RTC.h> 
 // #include <RTCZero.h>  
 // RTCZero RTC;  
 // #define RtcPresent                                                  
#endif

bool IsAfter(time_t t,time_t now)
{
  return (t > now) ;
}

bool IsBefore(time_t t,time_t now)
{
  return (t < now) ;
}

bool IsInPeriod(time_t t,time_t tstart,time_t tend)
{
  return (t > tstart && t < tend) ;
}

/*** NTP time  ****/
time_t NtpSetTime(char * NtpServer)
{
#if WiFiType > None
  const int timezone = 12;     // Fiji
  unsigned long epoch;
  const int updinterval = 43200000; // 12hrs in millis
  WiFiUDP ntpUDP;
                      //handle,server,offset,updateinterval
  NTPClient timeClient(ntpUDP, NtpServer, timezone * HrSec, updinterval);
  timeClient.begin();
  int cnt = 4;
  do
  {
    Dbg("NTP",cnt,1);
    timeClient.update(); 
  } while (!timeClient.isTimeSet() && cnt-- > 0);
  Dbg(timeClient.isTimeSet(),timeClient.getFormattedTime(),1);
  if (timeClient.isTimeSet())
    return timeClient.getEpochTime();
  else return 0; // return 0 if unable to get the time
#endif
}

bool ShowTime() {
  char dtstring[20];
  snprintf(dtstring,20,"%02i/%02i/%04i %02i:%02i:%02i",day(),month(),year(),hour(),minute(),second());
  Dbg(F("Time Is"),dtstring,1);
  return true;
}

time_t GetRtcTime()
{
  #if BoardType == TinyPico && defined RtcPresent
    return RTC.get();
  #elif BoardType >= PjcBegin && BoardType <= PjcEnd
    return Teensy3Clock.get(); 
  #else 
    return 0;
  #endif
}

void SetRtcTime(time_t t)
{
    #if BoardType == TinyPico && defined RtcPresent
       RTC.set(t);
    #elif BoardType >= PjcBegin && BoardType <= PjcEnd
       Teensy3Clock.set(t);
    #endif 
}

void SetDateTime()
{
  time_t t = 0;  
  #if IsEsp8266 > 0
    wdt_disable();
  #endif

  #if WiFiType > None && TryAps > 0
    Dbg(F("Trying"),F("NTP Server"),1);
    t = NtpSetTime("time.google.com");
  #else
    #if GprsPresent > None
      Dbg(F("Trying"),F("Gprs"),1);
      t = GprsSetTime();
     #else ;//nothing
     #endif
  #endif
  #if defined RtcPresent
     #if BoardType == TinyPico
       RTC.detect();
     #endif
     #if IsSamd > 0 
       RTC.begin(); // initialize RTC
     #endif
     setSyncProvider(GetRtcTime);  
  #endif
  if (t != 0) 
  { 
   #if defined RtcPresent
      Dbg(F("Setting"),F("RTC"),9);
      SetRtcTime(t); 
      //setSyncProvider(GetRtcTime);  
    #else 
      Dbg("call","setTime",1);
      setTime(t);
    #endif
   }
   else 
   { 
    Serial.println("Notimesource");
    // Dbg(F("No Available TimeSource"),F("1 am 1 jan 2020"),1);
     #if defined RtcPresent
       t = GetRtcTime();
     #else
       setTime(1, 0, 0, 1, 1, 20);
     #endif
   }
   #if IsEsp8266 > 0
     wdt_enable(WDTO_1S);
   #endif
  
  ShowTime(); 
}

#endif