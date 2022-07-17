#ifndef ZenUpdSrv
#define ZenUpdSrv


#if WiFiType == Esp8266 || WiFiType == Esp32
  #include <ESP8266httpUpdate.h>
#endif

float GetNewVer()
{
  float NewVer = 0;
  String payload = "0.0";
  WiFiClient client;
  HTTPClient http;
  if (http.begin(client, UpdServer))  // need ending / maybe
  {  
      Dbg(F("GET"),VerData,1);
      client.println(VerData); 
      int httpCode = http.GET(VerData);
  //http.open("GET", VerData, true);
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Dbg(F("GET code"), httpCode,1);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = http.getString();
          Dbg(F("Got"),payload,1);
        }
      } 
      else Dbg("F(GET Failed"), http.errorToString(httpCode).c_str(),1);
      NewVer = payload.toFloat();
      http.end();
  } 
  else Dbg(F("Server Not Found"),VerData,1);
  return NewVer;
}

bool WriteVersion(float Ver)
{
  File file = SPIFFS.open(UpdVerFile, "w");
  if (!file) {
    Dbg(F("Version file"),F("Open Fail"),1);
    return false;
  }
  if (!file.print(Ver) > 0) {
    Dbg(F("Version file"),F("Write Fail"),1);
    return false;
  }
  file.close();
  return true;
}

bool CheckHttpUpdateAvailable()
{
  float NewVer = GetNewVer();
  if (policy.Version < NewVer)
  {
    Dbg(F("Update To"),NewVer,1);
    // update from server
    WiFiClient client;
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, UpdData);
    switch (ret) 
    {
      case HTTP_UPDATE_FAILED:
        Dbg(F("Error"), ESPhttpUpdate.getLastErrorString().c_str(),1);
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Dbg(F("No Update Done"),"",1);
        break;
      case HTTP_UPDATE_OK:
        Dbg(F("Beginning Update to Version"),NewVer,1);
        // do the update thingy using VerData
        WriteVersion(NewVer);
        break;
    }
  }
  else Dbg("F(No Update Needed"),"",1);
}
#endif