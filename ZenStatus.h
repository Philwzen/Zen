#ifndef ZenStatus
#define ZenStatus
//Status stuff

int GetLed(int LedName)
{
 if (LedName < 0) return LedName;
 else return Leds[LedName];
}

void ShowStatus(int led,bool OnOff,int TriColour)
{   
  int CycleTime = 25; //ms   
  #if BoardType == TinyPico 
    TriColour = 1;
  #endif
  if (TriColour > 0) 
  {
    if (!OnOff) led = 0; 
    #if BoardType == TinyPico  
      switch(led)
      {
      case -2: tp.DotStar_CycleColor(CycleTime); break; //miliis
      case -1: tp.DotStar_Clear();  break;  // R  , G, B 
      case 0: tp.DotStar_SetPixelColor(0,0,0 );  break;    // off
      case 1: tp.DotStar_SetPixelColor(255,0,0); break;    // R
      case 2: tp.DotStar_SetPixelColor(0,255,0); break;    // G
      case 3: tp.DotStar_SetPixelColor(0,0,255); break;    // B
      case 4: tp.DotStar_SetPixelColor(255,255,0); break;  // O
      case 5: tp.DotStar_SetPixelColor(255,0,255); break;  // V
      case 6: tp.DotStar_SetPixelColor(0,255,255); break;  // T
      case 7: tp.DotStar_SetPixelColor(255,255,255); break;// W
      }
    #else // emulate tricilour led with 3 seperate leds how to cycle colour ??
      int R,G,B; // to mimic tp tricolor 
      R = Leds[RedLed];G = Leds[GreenLed];B = Leds[BlueLed];
      switch(led)
      {
      case -2: break;  // todocycle
      case -1: break;  // nothing 
      case 0: digitalWrite(R,LOW);digitalWrite(G,LOW);digitalWrite(B,LOW);  break;   // off
      case 1: digitalWrite(R,HIGH);digitalWrite(G,LOW);digitalWrite(B,LOW); break;   // R
      case 2: digitalWrite(R,LOW);digitalWrite(G,HIGH);digitalWrite(B,LOW); break;   // G
      case 3: digitalWrite(R,LOW);digitalWrite(G,LOW);digitalWrite(B,HIGH); break;   // B
      case 4: digitalWrite(R,HIGH);digitalWrite(G,HIGH);digitalWrite(B,LOW); break;  // O
      case 5: digitalWrite(R,HIGH);digitalWrite(G,LOW);digitalWrite(B,HIGH); break;  // V
      case 6: digitalWrite(R,LOW);digitalWrite(G,HIGH);digitalWrite(B,HIGH); break;  // T
      case 7: digitalWrite(R,HIGH);digitalWrite(G,HIGH);digitalWrite(B,HIGH); break; // W
      }
   #endif
  } // otherwise just light the led
  else if (led > 0) 
  {
    #if BoardType != Esp32Cam || FileSys != Sdcard
      pinMode(led,OUTPUT);
      digitalWrite(led,!OnOff);
    #endif
  }
}

#if WiFiType > None
   bool NetworkConnected();
   bool NetworkConnect(int Tries);
#endif

//  ****   ImAlive
bool tick = false;
int NetCheckCount = 0;

#if IsEsp8266 > 0 // its a esp8266
 void ICACHE_RAM_ATTR ImAlive()
#elif IsEsp32 > 0 // its a esp32
 void IRAM_ATTR ImAlive()
#else  // everything else ?? teensy
 void ImAlive()
#endif
// now the function logic   ImAlive
{
#ifndef NoHWTimers
 tick = !tick;
//  Dbg("Begin","ImAlive",1);
 #if BoardType != TinyPico
   ShowStatus(GetLed(AliveLed),tick,0);
 #endif
 #if IsEsp8266 > 0 // its a esp8266
   timer1_write(Policy.T0Interval * HWSec);
 #elif IsEsp32 > 0 // its a esp32
   #if BoardType == TinyPico
    if (tick) ShowStatus(GetLed(GreenLed),1,0);
    else ShowStatus(GetLed(OffLed),1,0);
   #endif
 #elif IsTeensy > 0   // teensy
    #if BoardType == Teensy32
       // Dbg("ImAlive","Teensy32",1);
       // no need to do anything
    #else  //T4
       // Dbg("ImAlive","Teensy4",1);
       // no need to do anything
    #endif
  #elif IsSamd > 0
//    Dbg("ImAlive","Samd",1);  
  #endif

 // rebootevery logic  check this 
 Policy.ReBootInterval = Policy.ReBootInterval - Policy.T0Interval;  // set interval to 0 to not reboot
 if (Policy.ReBootInterval < 0) ReBootMe();

 //Always do
 #if WiFiType > None && TryAps > 0 
   if (Policy.NetCheckInt > 0)
   {
     NetCheckCount++; 
     if (NetCheckCount >= Policy.NetCheckInt) 
     {
       Dbg("Check","NetWork",1);
       NetCheckCount = 0; 
       if (!NetworkConnected()) NetworkConnect(Policy.WiFiTries);
       #if BoardType == TinyPico
         ShowStatus(GetLed(BlueLed),1,0);
       #endif
      }
    }
 #endif
#endif
}

void FatalError(const char* In, const int Led)
{
    int x = 0;
    Dbg(F("Fatal Error"),In,9);
    Dbg(F(" System"),F("Halted"),9);
    #if RebootOnFatal > 0
      unsigned int Cntr;
      x = RebootOnFatal;
      Dbg(" Reboot In",x,9);
      Cntr = millis();
    #endif
    while (true && x > 0)
    {
      #if RebootOnFatal > 0 
        if (millis() - Cntr > 1000) {
          x--;
          Dbg(" Reboot In",x,9);
          Cntr = millis();
        }
      #endif
      ShowStatus(GetLed(Led),1,0);	
    }
    #if RebootOnFatal > 0 
      ReBootMe();
    #else
      while(true);
    #endif
}

void FatalError(const char* in)
{
    FatalError(in,0);		
}


#endif
