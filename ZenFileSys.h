#ifndef ZenFileSys
#define ZenFileSys

#if FileSys > None
#define FsErrorLed    VioletLed  //zenfilesys.h zenlog.h

//prepro for filesytemhandle 
#if FileSys == Spiffs
   #define fsh SPIFFS
   #define FILE_WRITE "w"
   #define FILE_READ  "r"
#elif FileSys == Sdcard
   #if BoardType == Esp32Cam
     #define fsh SD_MMC
     #include "FS.h"                // SD Card ESP32
     #include "SD_MMC.h"            // SD Card ESP32
   //  fsh::FS &fsh = SD_MMC;
   #else
     #define fsh SD
     #include <SPI.h>
     #include <SD.h>
   #endif
#else
   //Eeprom
#endif

#ifndef FsErrorLed
  #define  FsErrorLed    DummyLed
#endif

#if IsEsp > 0 && FileSys > None // all esp's
  #include <FS.h> // ?? SDFS.h as well
  #if IsEsp32 > 0 // esp32
    #include <SPIFFS.h>
  #endif
#endif

#if FileSys == Sdcard
  #ifndef SdPin
    #if IsEsp > 0 
      #if IsEsp32 > 0 //esp32
        #define SdPin 5
      #else                  // esp8266
        #define SdPin 15
      #endif
    #elif IsSamd > 0
      #define SdPin 4
    #else                    // teensy ,leo etc
      #define SdPin 4
    #endif
  #endif

  void SetSdPin()
  {
    pinMode(SdPin,OUTPUT);
    delay(500);
  }
#endif

void FileError(const char * err,const char * fname)
{
  Dbg(err,fname,1); 
  ShowStatus(GetLed(FsErrorLed),1,0);
  FSUp = false;
}


bool InitFileSys()
{
  int Tries = 0;
  const int MaxTries = 5;
  const int TryDelay = 200;
  #if FileSys == Eeprom  // not done yet
  //  EEPROM.setMemPool(memBase, UseEEprom);
  //  EEPROM.setMaxAllowedWrites(maxAllowedWrites);
    return true;
  #elif FileSys == Spiffs 
      #if IsEsp32 > 0
         if (!fsh.begin(true))
      #else 
         if (!fsh.begin())
      #endif
         {
          FileError("Cannot Start","SPIFFS");
          return false;
         }
         else return true;
  #elif FileSys == Sdcard
    #if BoardType > PjcBegin
      while (!fsh.begin(SPI_HALF_SPEED, SdPin) && Tries < MaxTries) 
    #elif BoardType == Esp32Cam
      while (!fsh.begin() && Tries < MaxTries) 
    #else
      while (!fsh.begin(SdPin) && Tries < MaxTries) 
    #endif
      {
        Tries++;
        Dbg(F("Init SD Card Retry"),Tries,9);
        delay(TryDelay);
      }
      if (Tries >= MaxTries ) 
      {
        FileError("Cannot Start","SDCARD");
        return false;
      }
      else 
      {
        // Dbg(F("SdCard Init"),F("OK"),1);
         return true; // 
      } 
  #endif
  return false; // will never get here
}

#if IsEsp8266 > 0 && FileSys == Spiffs
    Dir OpenDir(const char * fname) //Espiffs sp8266 only Dir handle
    {
      return fsh.openDir(fname);
    }
//    Dir CloseDir(Dir dir) //Espiffs sp8266 only Dir handle
//    {
//      return fsh.close(dir);
//    }
#else
    File OpenDir(const char * fname) 
    {
      return fsh.open(fname);
    }
    void CloseDir(File dir) //*****
    {
      dir.close();
    } 
#endif

String AddRoot()
{  // sometimes need to add "/" to prefix of filename
    #if FileSys == Spiffs || (IsEsp32 > 0) // && BoardType != Esp32Cam)
        return "/";
    #else
        return "";
    #endif 
}

File FileOpenWrite(const char * fname)
{   
   if (!FSUp) FSUp = InitFileSys();
   String fn = AddRoot() + fname;
 //   #if FileSys == Spiffs || (IsEsp32 > 0)
 //       String fn = "/";
 //       fn = fn + fname;
 //   #else
 //       String fn = fname;
 //   #endif 
 // use   fn.c_str() not fname
    File file;
    #if IsEsp32 > 0 // doesnt append by default!
      file = fsh.open(fn.c_str(),FILE_APPEND);
    #else
      file = fsh.open(fn.c_str(),FILE_WRITE);
    #endif 
//   file.seek(EOF);
    if (!file) FileError("Cannot Open For Write",fn.c_str());
    return file;
}

File FileOpenRead(const char * fname)
{ 
    if (!FSUp) FSUp = InitFileSys();
    String fn = AddRoot() + fname;
 //   #if FileSys == Spiffs || (IsEsp32 > 0)
 //       String fn = "/";
 //       fn = fn + fname;
 //   #else
 //       String fn = fname;
 //   #endif 
 // use   fn.c_str() not fname
    File file;
    file = fsh.open(fn.c_str(),FILE_READ);
    if (!file) FileError("Cannot Open For Read",fn.c_str());
    return file;
}

bool FileWrite(File file,unsigned char * data,unsigned int len)
{
  file.write(data,len);
  return true;
}

bool FileWriteLine(File file,const char * data)
{
   file.println(data);
   return true;
}

bool FileClose(File file)
{
   file.close();
   return true;
}

bool FileAvailable(File file)
{
   return file.available();
}

bool FileDelete(const char * fname)
{
    if (!FSUp) FSUp = InitFileSys();
    String fn = AddRoot() + fname;
 //   #if FileSys == Spiffs || (IsEsp32 > 0)
 //       String fn = "/";
 //       fn = fn + fname;
 //   #else
 //       String fn = fname;
 //   #endif 
 // use   fn.c_str() not fname
    return fsh.remove(fn.c_str());
}

char FileRead(File file)
{
  return file.read();
}

String FileReadLine(File file)
{ // read till next CRLF
  String Orec;
  int x = 0;
  char c;
  while (FileAvailable(file))
  {
   c = FileRead(file);
   if (c == 13 ) break;  // "\r") break;
   Orec += c;
   x++;
  }
  Orec.trim();
  return Orec;
}

String FileReadx(File file,int MaxChar)
{
  String Orec;
  int x = 0;
  char c;
  while (FileAvailable(file) && x < MaxChar)
  {
   c = FileRead(file);
 //  if (c == "\n\r") break;
   Orec += c;
   x++;
  }
  Orec.trim();
  return Orec;
}

int FileSize(const char * fname)
{
   int fsize;
   File file = FileOpenRead(fname);
   fsize = file.size();
   FileClose(file);
   return fsize;
}

String FileToString(const char * fname)
{
  String Scratch = "**error**";
  File fileIn  = FileOpenRead(fname); 
  if (!fileIn) return Scratch;
  Scratch.reserve(fileIn.size());
  Scratch = FileReadx(fileIn,fileIn.size());
  FileClose(fileIn);
  return Scratch;
}

bool FileExists(const char * fname)
{
   if (!FSUp) FSUp = InitFileSys();
   String fn = AddRoot() + fname;
 //   #if FileSys == Spiffs || (IsEsp32 > 0)
 //       String fn = "/";
 //       fn = fn + fname;
 //   #else
 //       String fn = fname;
 //   #endif 
 // use   fn.c_str() not fname
   return fsh.exists(fn.c_str());
}

int FileAppend(const char * FFrom,const char * FTo,bool DeleteFirst)
{
    File fileIn  = FileOpenRead(FFrom); 
    if (!fileIn) return 1;
    if (DeleteFirst) 
    {
      // delete destination first
      if (FileExists(FTo)) if (!FileDelete(FTo)) return 2;
    }
    File fileOut = FileOpenWrite(FTo); 
    if (!fileOut) return 3;
    String Scratch;
    Scratch.reserve(fileIn.size());
    Scratch = FileReadx(fileIn,fileIn.size());
    FileWriteLine(fileOut,Scratch.c_str());
    FileClose(fileIn);
    FileClose(fileOut);  
    return 0;    
}

bool FileRename(const char * from,const char *  to)
{
   if (!FSUp) FSUp = InitFileSys();
   String fn = AddRoot() + from;
   String tn = AddRoot() + to;
 //  #if FileSys == Spiffs || (IsEsp32 > 0 && BoardType != Esp32Cam)
 //       String fn = "/";
 //       String tn = "/";
 //       fn = fn + from;
 //       tn = tn + to;
 //   #else
 //       String fn = from;
 //       String tn = to;
 //   #endif 

   #if FileSys == Spiffs
     return fsh.rename(fn.c_str(),tn.c_str());
   #else
     if (FileAppend(fn.c_str(),tn.c_str(),true) == 0) // true del to first 0 no error
     {
       FileDelete(fn.c_str());
       return true;
     }
     else return false;
   #endif
}

void ListDir(const char * cwdName)  // eg ListDir("/");
{ 
  Dbg(F("Directory of"), cwdName, 1);
  String fn, fs;
  if (!FSUp) FSUp = InitFileSys();

  #if IsEsp8266 > 0 && FileSys != Sdcard 
    // esp8266 Spiffs only
    Dir dir = OpenDir(cwdName);
 //   if (!dir) Dbg("Esp8266 SPIFFS Can't open directory",cwdName,1);
 //   else 
 //   }
 //     while (dir.next())
 //     {
 //       fn = dir.fileName();
 //       fn.remove(0, 1);
 //       fs = String(dir.fileSize());
 //       Dbg(fn, fs, 1);
 //     }   
 //  //   DirClose(dir); 
 //    }  
  #elif FileSys == Spiffs || FileSys == Sdcard  // esp32 teensy
    File root = FileOpenRead(cwdName);
    if (!root) Dbg("Can't open directory",cwdName,1);
    else 
    { 
      File file = root.openNextFile();
      while(file)
      {
          fn = file.name();
          if (file.isDirectory()) fs = "<DIR>";
	  else fs = String(file.size());
          Dbg(fn, fs, 1);
          file = root.openNextFile();
      }
      FileClose(file);
    }
  #endif 
}

int FileListDir(const char * dir,String fspec,String *pDirList)
{ //pDirlist is output array of String[xxx] 
  // now add dir listing
  Dbg(dir,fspec,1);
  File root = FileOpenRead(dir);
  if (!root)
  {
    Dbg("Can't open Dir", dir, 1);
    return 0;
  }
  File file = root.openNextFile();
  if (!file)
  {
    Dbg(F("Empty Dir"),F("/"), 1);
    return 0;
  }
  // sort descending
  String fname;
  fspec.toLowerCase();
  fspec.trim();
  int NumItems = 0;
  while (file)
  { 
    fname = file.name();
    #if BoardType == Esp32Cam  // remove crap // from begining
      fname.remove(0,2);
    #endif 
    fname.toLowerCase();
 //   Dbg("Found",fname,1);
    // matches would be nice!
    if (fname.endsWith(fspec) || fspec == "*")
    {
      pDirList[NumItems] = fname;
      NumItems++;
    }
    FileClose(file);
    file = root.openNextFile();
  }
  FileClose(file);
  FileClose(root);
  Dbg(F("Matched"),NumItems,1);
  return NumItems;
}

  //Ftp Server stuff for all boards
  //esp32   sdcard #include <ESP32FtpSdSrv.h>
  //esp8266 sdcard #include <ESP8266FtpSdSrv.h>
  //esp32   spiffs #include <ESPFtpSpiffSrv.h>
  //esp8266 spiffs #include <ESPFtpSpiffSrv.h>
  //
  #if FtpType > None
    // eventually just use zenFtp for all
    #if FtpType == Esp8266Ftp 
      #if FileSys == Spiffs           // ok
        #include <ESPFtpSpiffSrv.h>
      #else
        #include <FtpZen.h>       // espsd ok
        // #include <ESP8266FtpSdSrv.h>  // ok
      #endif
    #elif FtpType == Esp32Ftp
      #if FileSys == Sdcard           // ok
        #include <FtpZen.h>           // EspSd ok
        // #include <ESP32FtpSdSrv.h>
      #else
        #include <ESPFtpSpiffSrv.h> // ok
      #endif
    #elif FtpType == Mkr1000Ftp 
      // #include <FtpServer.h> 
    #elif FtpType == ZenFtp 
      #include <FtpZen.h>      // Espsd ok Espspiffs no
    #elif FtpType == DefaultFtp 
      #include <FtpServer.h>         // ??
    #endif
    FtpServer FtpSrv;
  #endif

#endif // Filsys > none

#endif