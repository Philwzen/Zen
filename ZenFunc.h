#ifndef ZFunc
#define ZFunc

#include <ArduinoJson.h>
#include <ArduinoSort.h>

#ifndef DefaultAnalogNumSamples
  #define DefaultAnalogNumSamples 10
#endif

void FlashLed(int Led,int times,int interval)
{ // need this to start a timer not do aloop
   pinMode(Led, OUTPUT);
   digitalWrite(Led, HIGH);
   for(int i = 0; i < times; i++){
     digitalWrite(Led, !digitalRead(Led));
     delay(interval);
   } 
 //  AlarmId LedId = 0;
 //  LedId = Alarm.timerRepeat(ASec * Interval, LedProc); 
}  


int GetAnalogValue(int PinNum,int PinBias,bool CenterZero,int NumSamples)
{
  pinMode(PinNum,INPUT);
  int PinValue;
  // Processor dependant analog ref voltage and resolution
    #if IsAvr > 0 
    static const int AnalogRes = 1024;
    static const int ARRes     = 10;
  #elif IsEsp8266 > 0 || IsTeensy > 0
    static const int AnalogRes = 1024;
    static const int ARRes     = 10;  
  #elif IsEsp32 > 0 || IsSamd > 0 // 12bit
    static const int AnalogRes = 4096;
    static const int ARRes     = 12;
  #endif 
  #if IsSamd > 0 
    analogReadResolution(ARRes); 
  #endif
  //Average the readings
  if (NumSamples == 0) NumSamples = DefaultAnalogNumSamples;
  int ScratchValue = 0;
  int x = NumSamples;
  do //for NumSamples
  { 
    ScratchValue = ScratchValue + (analogRead(PinNum) - PinBias); // adjust any bias eg:esp32
    if (x > 1) delay(50); // let pin settle between reads
    x--;
  } while(x > 0);
  PinValue =  round(ScratchValue / NumSamples); 
  // zerooffset for negative measurements eg: -50a to +50a
  bool IsNeg = false;
  if (CenterZero)
  {
    static const int ZeroOffset  = AnalogRes / 2;
    if (PinValue < ZeroOffset) 
    {
      IsNeg = true;
      PinValue = ZeroOffset - PinValue;
    }
    else PinValue = PinValue - ZeroOffset;
  }
  if (IsNeg) PinValue = PinValue * -1;
 
  return PinValue;
}
 
float GetAnalogVoltage(int PinNum,int PinBias,bool CenterZero,float VRange,int NumSamples)
{
  // Processor dependant analog ref voltage and resolution
  #if IsAvr > 0 
    static const float AnalogRefV  = 5.0;         
    static const int   AnalogRes   = 1024;
  #elif IsEsp8266 > 0 || IsTeensy > 0
    static const float AnalogRefV  = 3.3; 
    static const int   AnalogRes   = 1024;
  #elif IsEsp32 > 0 || IsSamd > 0 // 12bit
    static const float AnalogRefV  = 3.3; 
    static const int   AnalogRes   = 4096;
  #endif
                               //int ,int ,bool ,int ,int
  int PinValue = GetAnalogValue(PinNum,PinBias,CenterZero,NumSamples);
  // scale to usefull numbers
  float ReturnValue = (PinValue * AnalogRefV) / AnalogRes;
  ReturnValue = ReturnValue / (AnalogRefV/VRange);
 return ReturnValue;
}

float GetAnalogAmps(int AnalogPin,int AMax,int NumSamples)
{
  int mVperAmp;
  switch (AMax)
  { // 185 = ACS712 5A, 100 = ACS712 20A, 66 = ACS712-30A
    case 5:
      mVperAmp   = 185;
      break;
    case 20:
      mVperAmp   = 100; 
      break;
    case 30:
      mVperAmp   = 66;
      break; 
    default:
      mVperAmp   = 66;
      break;
  }
  #if IsAvr > 0 
    static const float AnalogRefV  = 5.0;         
    static const int   AnalogRes   = 1024;  
  #elif IsEsp8266 > 0 || IsTeensy > 0
    static const float AnalogRefV  = 3.3; 
    static const int   AnalogRes   = 1024;
  #elif IsEsp32 > 0 || IsSamd > 0 // 12bit
    static const float AnalogRefV  = 3.3; 
    static const int   AnalogRes   = 4096;
  #endif
   
   float RawVolts = GetAnalogVoltage(AnalogPin,0,0,AnalogRefV,NumSamples);
   Dbg("Raw Volts",RawVolts,0); 
  
   return ((RawVolts*1000)/ mVperAmp);   
}
/*****************************************************/
// String handling
#define STRINGIFY(s) XSTRINGIFY(s)
#define XSTRINGIFY(s) #s

String ToString(char * buff)
{
  String str = buff;
  return str;
}

bool IsNumeric(String string) {
  for (unsigned int i = 0; i < string.length(); ++i) {
    if (!isdigit(string[i]))
      return false;
  }
  return true;
}

String SUnQuote(String data)
{
  int x = 0;	
  while(true)
  {
    x = data.indexOf('"');
    if (x < 0) break;	
    data.remove(x, 1);
  }
  data.trim();
  return data;
} 

void ToCharArray(String ipstring,char * obuff)
{
  ipstring.trim();
  ipstring.toCharArray(obuff, ipstring.length() + 1);
}

void CUnQuote(char * ibuff)
{
  String data = ibuff;
  int x = 0;	
  while(true)
  {
    x = data.indexOf('"');
    if (x < 0) break;	
    data.remove(x, 1);
  }
  data.trim();
  data.toCharArray(ibuff, data.length());
} 
/*
int NUmItems(String list,char * delim)
{
 int x = 0;
 while (GetElement(list,x,delim) != "")
 {
    x++;
 };
 return x;
}
*/
int GetIndexOf(String array[],String Target){
   int counter = 0;
   while (array[counter] != "\0")
   {
//     Dbg(array[counter],counter,1);
     if (array[counter] == Target)
        return counter;
     else   
        counter++;
   }
   return -1;
}

String GetItem(String IStr,int index,char Delim)
{
  char ibuff[255];
  ToCharArray(IStr,ibuff);

  //IStr.trim();
  //IStr.toCharArray(ibuff, IStr.length() + 1);

  char *token = strtok(ibuff, &Delim);
  int x = 0;
  while (token != NULL && x != index) 
    { 
      token = strtok(NULL, &Delim);
      x++; 
    } 
  String Orec = token;
  return Orec;
}

String GetIniVar(char * fname,String VarName,char Delim)
{
  String Irec;
  String VValue = "Not Found";
#if FileSys > None
  File DataFile = FileOpenRead(fname);
  while (!Irec.equals("EOF"))
  {
    Irec = FileReadLine(DataFile);
    if (GetItem(Irec,0,Delim) == VarName )
    {   
      VValue = GetItem(Irec,1,Delim); 
      break; 
    }
  }
  FileClose(DataFile);
#endif
  return VValue;
}

template< typename S, typename T, typename U > String GetElement( S ibuff, T index, U Delim );

template< typename S, typename T, typename U > String GetElement(  S ibuff, T index, U Delim )
//String GetElement(char * ibuff,int index,char * Delim)
{
  String data = ibuff;
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==Delim || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
/*******************************************/
bool ToBool(String val)
{
 String Trues[]  = {"1","true","ok","yes","\0"};
 String Falses[] = {"0","false","!ok","nok","no","\0"};
 val.toLowerCase();
 bool result = (GetIndexOf(Trues,val) > -1);
 return result;
}

/*******************************************/
void DisplayPolicy(String from)
{
  Dbg(F("Version"),Policy.Version,1);
  Dbg(F("UseSecurity"),Policy.UseSecurity,1);
  Dbg(F("AdminList"),Policy.AdminList,1);
  Dbg(F("LogAll"),Policy.LogAll,1);
  Dbg(F("ReBootInterval"),Policy.ReBootInterval,1);
  Dbg(F("T0Interval"),Policy.T0Interval,1);
  Dbg(F("WiFiTries"),Policy.WiFiTries,1);
  Dbg(F("NetCheckInterval"),Policy.NetCheckInt,1);
  Dbg(F("Loaded Policies"),from,1);
}


String ReadInFile(char * fname)
{
    String data = "Failed";
    #if FileSys > None
      data = FileToString(fname);
    #endif
    return data;
}

String GetPolicyItem(char * fname,String ItemName)
{
   DynamicJsonDocument data(1024);
   DeserializationError error = deserializeJson(data,ReadInFile(fname));
   return data[ItemName].as<String>();
}

void DefaultPolicy()
{ 
   Policy.Version        = PPVersion;
   Policy.AdminList      = PPAdminList;
   Policy.LogAll         = PPLogAll;
   Policy.ReBootInterval = PPReBootInterval;  
   Policy.T0Interval     = PPT0Interval;
   Policy.WiFiTries      = PPWiFiTries;
   Policy.NetCheckInt    = PPNetCheckInt;
   
   #ifdef PPSecurity       
     Policy.UseSecurity    = true;
   #else Policy.UseSecurity    = false;
   #endif 
   #if WiFiType == AirLift
     Policy.NetCheckInt= 0;
   #else Policy.NetCheckInt    = PPNetCheckInt;
   #endif
}


SysVar LoadPolicies(char * fname)
{    
    String ptype;
    DefaultPolicy();

    #if FileSys > None
      StaticJsonDocument<512> data;
      DeserializationError error = deserializeJson(data, ReadInFile(fname));
      if (error) 
      {
        Dbg(F("Policy Error"),fname,0);
        Dbg(F("  Deserialization"),error.c_str(),1);
        ptype = "Default";
      }
      else
      {
       Policy.Version        = data["Version"];
       Policy.UseSecurity    = data["Security"];
       Policy.AdminList      = data["Administrators"].as<String>();
       Policy.LogAll         = data["LogAll"];
       Policy.ReBootInterval = data["ReBootInterval"];
       Policy.T0Interval     = data["T0Interval"];
       Policy.WiFiTries      = data["WiFiTries"];
       Policy.NetCheckInt    = data["NetCheckInterval"] ;
       ptype = fname;
      }
    #endif
    #ifndef PPSecurity 
      Policy.UseSecurity    = false;
    #endif
    #if WiFiType == AirLift
     Policy.NetCheckInt= 0;
    #else Policy.NetCheckInt    = PPNetCheckInt;
    #endif
    DisplayPolicy(ptype);

    return Policy;
}

bool IsBetween(int from,int Val,int to)
{
 return (Val >= from) && (Val <= to);
}

#endif