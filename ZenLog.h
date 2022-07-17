#ifndef ZLog
#define ZLog
#if defined PBScenario
   void SendToPb(String Scenarioid,String DataString);
#endif
#if IsAvr == 0
void LogIt(String logname,String P1,String P2 ,String P3 ,String P4)
{
  char TimeStamp[22];
  sprintf(TimeStamp,"%02i/%02i/%04i*%02i:%02i:%02i",day(),month(),year(),hour(),minute(),second());
  #if FileSys > None
     #define Delim1 "**"
     #define Delim2 "*"
     #define Delim3 "/"
 
     char fname[13]; 
     String Orec;

     memset(fname,char(0),12);
    
    if (logname.length() == 0) // default name if blank passed in
    {
      if (!Policy.LogAll) return;
      snprintf(fname,13,"%04i%02i%02i.log",year(),month(),day());
    }
    else 
    {
      //char* c = const_cast<char*>(logname.c_str());
      snprintf(fname,12,"%s.log",logname.c_str());
     }
     Orec = ToString(TimeStamp) + Delim1 + P1 + Delim2 + P2 + Delim2 + P3 + Delim2 + P4 + Delim1;
     Dbg(F("Loging to Filename"),fname,1);
     File file = FileOpenWrite(fname); 
     if (!file)  FatalError("Cannot Open Log File",FsErrorLed);
     FileWriteLine(file,Orec.c_str());
     FileClose(file);
     Dbg("Logged",Orec, 9);
   #else Dbg("Log","No Filesystem");
   #endif 
 
   #if defined PBScenario
     String Var[10];
     String Val[10];
     String ScenarioId = PBScenario;
     Var[0] = "TimeStamp"; Val[0] = ToString(TimeStamp);
     Var[1] = "Number";    Val[1] = P4;
     Var[2] = "Action";    Val[2] = P2;
     Var[3] = "Result";    Val[3] = P2;
    // SendToPb(ScenarioId,Var,Val);
   #endif     
}

void PurgeLog(char * filename)
{
 #if FileSys > None
  if (FileDelete(filename))
  {
    File file = FileOpenWrite(filename); 
    FileClose(file); 
    Dbg(F("Purge Complete"),filename,1); 
  }
 #endif     
  Dbg(F("Purge Failed"),filename,1);  
}

void ReadLog(char * filename,char * Phone)
{
#if FileSys > None
  File file = FileOpenRead(filename);
  FileReadx(file,80);
#endif
}

#else
void LogIt(String logname,String P1,String P2 ,String P3 ,String P4)
{
  Dbg(F("Log",P1,1);
}
#endif
#endif
