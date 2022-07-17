#ifndef ZenWebFunc
#define ZenWebFunc
#include <ArduinoJson.h>
#include <ArduinoSort.h>

void returnOK() 
{
  WebSrv.send(200, "text/plain", "");
}

void DoNothing()
{
}

void returnFail(String msg) {
  WebSrv.send(500, "text/plain", msg + "\r\n");
}

void handleNotFound()
{
  Dbg("Begin","handleNotFound",1);  
  String path = WebSrv.uri();
  Dbg("In Not Found",path,1);  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += path;
  message += "\nMethod: ";
  message += (WebSrv.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += WebSrv.args();
  message += "\n";

  for (uint8_t i = 0; i < WebSrv.args(); i++)
  {
    message += " " + WebSrv.argName(i) + ": " + WebSrv.arg(i) + "\n";
  }

  WebSrv.send(404, "text/plain", message);
  Dbg("Done","handleNotFound",1);  
}

void SrvStreamFile() 
{  
  Dbg("Begin", "SrvStreamFile", 1);
#if FileSys > None
  String SendFile = WebSrv.arg("lst");
  SendFile.trim();
  Dbg("SendFile", SendFile, 1);
  SendFile =  FileToString(SendFile.c_str());
  Dbg("Sending", SendFile.length(), 1);
  WebSrv.send(200, "application/octet-stream", SendFile);
#endif
  Dbg("Done", "SrvStreamFile", 1);
}

void SrvSendFile(String fname, String type)
{
#if FileSys > None
  File file = FileOpenRead(fname.c_str());
  if (file > 0)
  {
    #if WiFiType !=  None // MkrWiFi101
      String FileContents;
      Dbg("Send",fname,1);
      FileContents.reserve(file.size());
      FileContents = FileReadx(file,file.size());
      FileContents.trim(); 
      WebSrv.send(200, type, FileContents); 
      Dbg("SentSize",FileContents.length(),1);
      FileClose(file);
    #else            // dont think this streamFile works!!
      int x = WebSrv.streamFile(file, type);
      if (x != file.size()) 
      {
        Dbg("Sent", x, 1);
        Dbg("Size", file.size(), 1);
      }
     FileClose(file);
    #endif
  }
  else Dbg("Cant Load",fname,1);
#else // fname is the actual html text of the webpage
  WebSrv.send(200,type,fname);
#endif 
//Dbg("SrvSent",fname,1); 
}

void SendFile()
{
  Dbg("Begin", "SendFile", 1);
  String fname = WebSrv.arg("lst");
  fname.trim();
  #if FileSys > None
    SrvSendFile(fname,"text/plain");
  #endif 
 Dbg("Done", "SendFile", 1);
}

void SrvSendJsonDirList(char * indir,char * fspec,char * ArrayName)  // send list of Dir
{
  Dbg("Begin", "SrvSendJsonDirList", 1);
  char scratch[700];
  StaticJsonDocument<700> JsonData;
  JsonData["Ver"] = Policy.Version;
  
  String DirList[50];
  #if FileSys > 0
    int NumItems = FileListDir(indir,fspec,&DirList[0]);
  #else
    int NumItems = 0; 
  #endif 
  sortArrayReverse(DirList, NumItems); // array,numelements
  JsonArray List = JsonData.createNestedArray(ArrayName);
  for (int x = 0 ; x < NumItems ; x++)
  {
    DirList[x].trim();
    List.add(DirList[x]);
  }
  serializeJson(JsonData, scratch);
  Dbg("Json",scratch,1);
  WebSrv.send(200, "application/json",  scratch);

  Dbg("Done", "SrvSendJsonDirList", 1);
}

void LoadIndexRoot() {
   Dbg("Begin","LoadIndexRoot",1);
//   #if FileSys > None
     SrvSendFile(RootPage,"text/html");
 //  #else 
 //   String out;
 //   out.reserve(2048);
 //   out = RootPage;
 //   out.trim();
 //   WebSrv.send(200, "text/html", out);
 //   Dbg("Sentsize",out.length(),1);
 //  #endif
   Dbg("Done","LoadIndexRoot",1);
//  LoadFile(WebSrv.uri()); // dont work
}

/****************************** not used
bool LoadFile(String path) 
{
  Dbg("In Not LoadFile",path,1);
  if (path.startsWith("/favicon.ico")) return true;
  String dataType = "text/plain";
  // check this if esp sites play up
  #if FileSys > Eeprom
     if (path.endsWith("/"));
     { 
       path += RootPage;
     }
  #endif 
  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  #if FileSys > None
   path = RootPage;
   File dataFile = FileOpenRead(path.c_str());
   if (dataFile.isDirectory()) {
     #if FileSys == Sdcard
       path += "\" + RootPage;
     #else
       path += "/" + RootPage;    
     #endif
     dataType = "text/html";
     dataFile = FileOpenRead(path.c_str());
   }

   if (!dataFile) {
    Dbg("no file", path, 1);
    return false;
   }

   if (WebSrv.streamFile(dataFile, dataType) != dataFile.size()) {
    Dbg("Sent less data than expected!", dataFile.size(), 1);
   }
   dataFile.close();
  #endif
  return true;
}


void handleFileUpload() {
  if (WebSrv.uri() != "/edit") {
    return;
  }
  
#if FileSys > None
  HTTPUpload& upload = WebSrv.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (SD.exists((char *)upload.filename.c_str())) {
      SD.remove((char *)upload.filename.c_str());
    }
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
    Dbg("Upload: START, filename",upload.filename,1);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    Dbg("Upload: WRITE, Bytes",upload.currentSize,1);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    Dbg("Upload: END, Size",upload.totalSize,1);
  }
#endif
}

void deleteRecursive(String path) {
#if FileSys > None
  File file = SD.open((char *)path.c_str());
  if (!file.isDirectory()) {
    file.close();
    SD.remove((char *)path.c_str());
    return;
  }

  file.rewindDirectory();
  while (true) {
    File entry = file.openNextFile();
    if (!entry) {
      break;
    }
    String entryPath = path + "/" + entry.name();
    if (entry.isDirectory()) {
      entry.close();
      deleteRecursive(entryPath);
    } else {
      entry.close();
      SD.remove((char *)entryPath.c_str());
    }
    yield();
  }

  SD.rmdir((char *)path.c_str());
  file.close();
#endif
}

void handleDelete() {
  if (WebSrv.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = WebSrv.arg(0);
#if FileSys > None
  if (path == "/" || !SD.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }
  deleteRecursive(path);
#endif
  returnOK();
}

void handleCreate() {
  if (WebSrv.args() == 0) {
    return returnFail("BAD ARGS");
  }
  String path = WebSrv.arg(0);
#if FileSys > None
  if (path == "/" || SD.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }

  if (path.indexOf('.') > 0) {
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    if (file) {
      file.write((const char *)0);
      file.close();
    }
  } else {
    SD.mkdir((char *)path.c_str());
  }
#endif
  returnOK();
}

void printDirectory() {
  if (!WebSrv.hasArg("dir")) {
    return returnFail("BAD ARGS");
  }
  String path = WebSrv.arg("dir");
#if FileSys > None
  if (path != "/" && !SD.exists((char *)path.c_str())) {
    return returnFail("BAD PATH");
  }
  File dir = SD.open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  WebSrv.setContentLength(CONTENT_LENGTH_UNKNOWN);
  WebSrv.send(200, "text/json", "");
  WiFiClient client = WebSrv.client();

  WebSrv.sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }

    String output;
    if (cnt > 0) {
      output = ',';
    }

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\"";
    output += "}";
    WebSrv.sendContent(output);
    entry.close();
  }
  WebSrv.sendContent("]");
  WebSrv.sendContent(""); // Terminate the HTTP chunked transmission with a 0-length chunk
  dir.close();
#endif
}
***/
#endif