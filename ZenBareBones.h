
#ifndef ZenBareBones
#define ZenBareBones
void FlashLed(int Led,int times,int interval);

//#include <ZenForwardDefs.h>
void LogIt(String logname,String P1,String P2 ,String P3 ,String P4);
bool LoadPolicies(String policyfile);  
void FatalError(const char* In);
void FatalError(const char* In,int Led);

#if BoardType == TinyPico
  #include <TinyPICO.h>
  TinyPICO tp = TinyPICO();
#endif

#include <Zmsg.h>

int t;

// Watchdog 
#if IsAvr > 0
  #include <avr/wdt.h>  
#elif IsSamd > 0 
  #include <WDTZero.h>
  WDTZero MkrDog;
#elif IsEsp32 > 0
  #include <esp_task_wdt.h>
#endif

// ReBootme logic
void ReBootMe()
{ 
  Dbg("RebootMe",BoardClass,0);
  #if defined PreReboot
    PreReboot();
  #endif
  //FlashLed(AliveLed,10,200);
  #if IsAvr > o
      wdt_enable(WDTO_120MS);
      while(1) {};
  #elif IsEsp > 0
      ESP.restart();
  #elif IsTeensy > 0 //teensy  only works on 3.2
      #define RESTART_ADDR       0xE000ED0C
      #define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
      #define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
      // 0000101111110100000000000000100
      // Asesrt [2]SYSRESETREQ
      WRITE_RESTART(0x5FA0004);
  #elif IsSamd > 0 
     NVIC_SystemReset();
  #else 
      return;
  #endif
}

void SetWDog(int h, int l)  // samd and Teensy watchdog
{
#ifndef NoDog
 // Dbg(F("Set Watch"),F("Dog"),1);
  #if BoardType == Teensy32  // ??? teensy4 ????
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
    delayMicroseconds(1); // Need to wait a bit..
    WDOG_STCTRLH = 0x0001; // Enable WDG
    WDOG_TOVALL = l; // The next 2 lines sets the time-out value.
    WDOG_TOVALH = h;
    WDOG_PRESC = 0; // This sets prescale clock so that the watchdog timer ticks
                  // at 1kHZ instead of the default 1kHZ/4 = 200 HZ
  #elif IsAvr > 0
    if (WDTCSR & (1<<WDIE)) 
    {
  //    Dbg(F("WD"),F("Reset"),1);
      wdt_reset(); //enabled Prolong timer
    }
    else if (WDTCSR & (1<<WDE)) { Dbg("WD","Nothing",1);} //Bootloader reset do nothing
    else 
    {
 //     Dbg(F("WD"),F("Enable"),1);
      wdt_enable(WDTO_8S); //Watchdog disabled - Enable again
    }
  #elif IsSamd > 0 
     MkrDog.attachShutdown(ReBootMe);
 //    Dbg(F("WD"),F("Enable"),1);
     MkrDog.setup(WDT_SOFTCYCLE8S);  // initialize WDT-softcounter refesh cycle on 32sec interval
  #elif IEsp32 > 0
    esp_task_wdt_init(l * 10, true); //enable panic so ESP32 restarts
    esp_task_wdt_add(NULL); //add current thread to WDT watch
  #endif
#endif
}

void KickDog()
{
#ifndef NoDog
  #if BoardType == Teensy32  // ???? teensy4  ???
    noInterrupts();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    interrupts()
  #elif IsAvr > 0
    wdt_reset();
  #elif IsSamd > 0 
    MkrDog.clear();  
  #elif IsEsp32 > 0
    esp_task_wdt_reset();
  #endif
//  Dbg(F("Kicked Watch"),F("Dog"),1);
#endif
}

void ET()
{
  t = millis();
}
void ET(String In)
{
  Dbg(In + " Elapsed",(millis() - t),1);
}

#include <ZenTimers.h>   // Timers
#include <ZenStatus.h>   // Status stuff

#if FileSys > None
 #include <ZenFileSys.h>
#endif

#include <ZenFunc.h>     // usefull functions
#include <ZenLog.h>	 // loging stuff
#include <Z433.h>	 // 433mhz remote
#include <ZenUser.h>     // Users
#endif