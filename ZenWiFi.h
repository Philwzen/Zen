#ifndef ZenWiFi
#define ZenWiFi

#define WiFiErrorLed  OrangeLed  //zenwifi.h
 
#if WiFiType <= SpiEsp8266   //IsEsp etc
  #include <Esp/ZenEspWiFi.h>
#elif WiFiType < Cc3000      //IsSamd
  #include <Samd/ZenSamdWiFi.h>
#elif WiFiType < Rj45        //cc3000o IsAvr
  #include <Avr/ZenArWiFi.h>  
#else //Rj45
  //#include <ZenRj45.h>
#endif

//start appropriate webserver
#ifndef NoHttp // && WiFiServer > None
  WiFiServer WebSrv(HttpPort);
#endif

// update dynmic dns
#if defined UseDdns  // will need http 
  #include <EasyDDNS.h>
//  WiFiServer Ddnsserver(HttpPort)
  void SetDdns()
  {
    String DdnsHostName = "elysium.redirectme.net";
    Dbg("Setting DDNS",DdnsHostName,1);
 //   Ddnserver.begin();
    EasyDDNS.service("noip");
    EasyDDNS.client(DdnsHostName, "philwzen", "Crasies123");

    // Get Notified when your IP changes
    EasyDDNS.onUpdate([&](const char* oldIP, const char* newIP)
    {
      Dbg("EasyDDNS - IP Change Detected",newIP,1);
    });
    EasyDDNS.update(10000);  // can put in main loop if required 
  }
#endif

#ifndef WiFiErrorLed
   #define  WiFiErrorLed  DummyLed
#endif

#define WIFI_CONNECT_TIMEOUT 8000
#if WiFiType != Cc3000
  #include "WiFiAutoSelect.h"
  WiFiAutoSelector wifiAutoSelector(WIFI_CONNECT_TIMEOUT); 
#endif
#ifndef WebRootFunction
  #define WebRootFunction LoadIndexRoot
#endif
#ifndef RootPage
  #define RootPage "index.htm"
#endif

void WebRootFunction(); // prepro to set handleroot function
#ifndef NoHttp
  #include <ZenWebFunc.h>
#endif

String GetMyIp()
{
  String Ip;
 // depends on WiFiType and pos WiFiMode 
  #if WiFiType == AtEsp8266 || WiFiType == AirLift || WiFiType == MkrWiFi101 || WiFiType == Cc3000
    #if WiFiType == Cc3000
      uint32_t ip, netmask, gateway, dhcpserv, dnsserv;
      WiFi.getIPAddress(&ip, &netmask, &gateway, &dhcpserv, &dnsserv);
      Ip = String((uint8_t)(ip)) + '.' + String((uint8_t)(ip >> 8)) +  '.' + String((uint8_t)(ip >> 16)) + '.' + String((uint8_t)(ip >> 24));
    #else
      IPAddress ip = WiFi.localIP();
      for (int i=0; i<4; i++)
      Ip += i  ? "." + String(ip[i]) : String(ip[i]);
    #endif
  #else
    if (WiFi.status() == WL_DISCONNECTED) // in apmode
      Ip = WiFi.softAPIP().toString(); 
    else
      Ip = WiFi.localIP().toString();
  #endif
  return Ip;
}

// Wifi Stuff
int8_t GetWifiQuality()
{
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}

/* genericise the srv startup calls
#ifndef NoHttp
  bool WebSrvBegin() // notused yet
  {
    bool stat = 1;
    switch (WiFiType)
    {
     case ??: WebSrv.begin(httpport);
     default: WebSrv.begin();
    }
    Dbg("WebSrv ", SrvName, 1);
    return stat;
  }
#endif
#if FtpType > None
  bool FtpSrvBegin()  // Not Used Yet
  {
    bool stat = 1;
    switch (FtpType) 
    {
     default: FtpSrv.begin("Ftp","Ftp");
    }
    Dbg("FtpSrv ", SrvName, 1);
    return stat;
}
#endif
*/
#define str(x) #x 
void SetupServers()
{
  #ifndef NoHttp
    WebSrv.onNotFound(handleNotFound);
    WebSrv.on("/",WebRootFunction);
//?? additional webserver ons  ??? 
    WebSrv.begin();   // WebSrvBegin();
    Dbg("WebSrv Started", SrvName, 0);
    Dbg(" RootFunction",str(WebRootFunction),0);
    Dbg(" RootPage",RootPage,1);
  #else 
    Dbg("Not Starting","WebSrv",1);
  #endif
  #if FtpType > None
    FtpSrv.begin("Ftp","Ftp");   //  FtpSrvBegin()
    Dbg("FtpSrv Started", "Ftp,Ftp", 1);
  #else 
    Dbg("Not Starting","FtpSrv",1);
  #endif
  #ifndef NoMdns  // mdns doesnt really work
    Dbg("SrvName",SrvName,1);
    String SName = SrvName;
    SName.toLowerCase();
    if (MDNS.begin(SrvName)) 
    {
      #if BoardType < Esp8266
        MDNS.enableWorkstation(ESP_IF_WIFI_STA);
      #endif   
      MDNS.addService(SName, "_tcp", 8080); 
      #ifndef NoHttp
        MDNS.addService("http", "_tcp", 80);
      #endif
      #if FtpType > None
        MDNS.addService("ftp", "_tcp", 21);
      #endif
      Dbg("MDNS","Started",1);
    }
    else Dbg("MDNS","Failed to Start",1);
  #else
    Dbg("Not Starting","MdnsSrv",1);
  #endif
}

char* WiFiError (int errnum)
{
  static char emess[25];
  switch (errnum)
  {
    #if WiFiType == Cc3000
      case WLAN_STATUS_DISCONNECTED:{ strcpy(emess, "Disconnected"); break;}  // (0)
      case WLAN_STATUS_SCANNING:{ strcpy(emess, "Scanning"); break;}        //(1)
      case WLAN_STATUS_CONNECTING:{ strcpy(emess, "Connecting"); break;}      //(2)
      case WLAN_STATUS_CONNECTED:{ strcpy(emess, "Connected"); break;}       //(3)
    #else
      case WL_IDLE_STATUS:{ strcpy(emess, "Idle"); break;} 		// 0
      #if WiFiType != AtEsp8266
        case WL_NO_SSID_AVAIL:{ strcpy(emess, "No SSID Available"); break;} //1
        case WL_SCAN_COMPLETED:{ strcpy(emess, "Scan Complete"); break;}  	//2
        case WL_CONNECTION_LOST:{ strcpy(emess, "Connection Lost"); break;}  //5
      #endif
      case WL_CONNECTED:{ strcpy(emess, "Connected"); break;}  		//3
      case WL_CONNECT_FAILED:{ strcpy(emess, "Connection Failed"); break;} //4
      case WL_DISCONNECTED:{ strcpy(emess, "Disconnected"); break;}  	//6
      #if BoardType < Esp8266 
        case WL_AP_LISTENING:{ strcpy(emess, "AP Listening"); break;}  	//7
        case WL_AP_CONNECTED:{ strcpy(emess, "AP Connected"); break;}  	//8
        case WL_AP_FAILED:{ strcpy(emess, "AP Failed"); break;}  		//9
      #endif  
      case WL_NO_SHIELD:{ strcpy(emess, "No Board"); break;}          	// 255
    #endif
    default:
	{
	   strcpy(emess,"Unknown Error Code :");
	  // strcat(emess,errnum);
	   break;
	}
  }
  return emess;
}

bool StartAP(char * ssid,char * password)
{
 #if WiFiType != Cc3000
   #ifdef StaticApid  // must def StaticGway and StaticSnet
     IPAddress ApIp(StaticApid);
      IPAddress Gway(StaticGway);
    IPAddress Snet(StaticSnet);  
   #else
     IPAddress ApIp(192,168,0,1);
     IPAddress Gway(192,168,0,1);
     IPAddress Snet(255,255,255,0);
   #endif

   Dbg("Creating Access Point",ssid,0);

   #if WiFiType == AtEsp8266
     WiFi.configAP(ApIp);
     delay(500);
     int AP_channel = 10;
     char s[10]; 
     char p[10];
     strcpy(s, ssid);    
     strcpy(p, password);
     if (password == "")
       WiFi.beginAP(s); 
     else 
       WiFi.beginAP(s, AP_channel, p, ENC_TYPE_WPA2_PSK);
   #elif WiFiType == AirLift || WiFiType == MkrWiFi101
     if (WiFi.beginAP(ssid,password) != WL_AP_LISTENING)
       return false;
     delay(500);
     WiFi.config(ApIp,Gway,Gway,Snet);
   #else
     WiFi.mode(WIFI_OFF);
     delay(500);
     WiFi.mode(WIFI_AP);
     delay(500);
     if (!WiFi.softAP(ssid,password)) return false;
     delay(1000);
     WiFi.softAPConfig(ApIp,Gway,Snet);
   #endif
   delay(500);

   Dbg(" @",GetMyIp(),1);
   Policy.NetCheckInt = 0;  // no need to check network
   Dbg(ssid,"Created OK",1);
   SetupServers();
   return true;  
 #else
   FatalError("CC3000 NO AP MODE",WiFiErrorLed);
   return false; 
 #endif
}

bool NetworkConnect(int Tries)
{
  int y = 0;
  while (y < Tries)
  {  // would like to include option to not use autoconnect stuff
   #if WiFiType != Cc3000
    if(-1 < wifiAutoSelector.scanAndConnect()) 
    {
      #ifdef StaticIpid
        if (strcmp(ssid,"Marvin") != 0) // no static ip on marvin
        {
          IPAddress ApIp(StaticIpid);
          #if WiFiType == AtEsp8266
            WiFi.config(ApIp); 
          #else
            IPAddress Gway(StaticGway);
            IPAddress Snet(StaticSnet); 
            WiFi.config(ApIp, Gway, Snet, Gway);
          #endif
        }
      #endif
      int connectedIndex = wifiAutoSelector.getConnectedIndex();
  //    Dbg("WiFi Status",WiFiError(WiFi.status()),1);
      Dbg("Connected To",wifiAutoSelector.getSSID(connectedIndex),0);
   #else
    if (WiFi.connectToAP(ssid[y], pass[y], WLAN_SEC_WPA2))
    {
   #endif
      Dbg(" @",GetMyIp(),1);   
      SetupServers();
      return true;
    }
    else 
    { 
      y++;
 //     Dbg("WiFi Status",WiFiError(WiFi.getStatus()),1)
      Dbg("WiFi Status",WiFiError(WiFi.status()),1);
      Dbg("Network Retry",y,1);
    }
  }
  return false;
}

bool NetworkConnected()
{
  bool stat = false;
  int wstatus = WiFi.status();
  switch (wstatus)
  #if true == false  //commentout
    { // ?? are they the same in AP mode ??
    #if WiFiType == MkrWiFi101
      case WL_NO_SHIELD:      {stat = false; break;}
      case WL_CONNECTED:      {stat = true;  break;} // 3
      case WL_CONNECT_FAILED: {stat = false; break;} // 4
      case WL_DISCONNECTED:   {stat = false; break;} // 6
    #elif WiFiType == Cc3000
      case WLAN_STATUS_DISCONNECTED: {stat = false; break;}  // (0)
      case WLAN_STATUS_SCANNING:     {stat = false; break;}  //(1)
      case WLAN_STATUS_CONNECTING:   {stat = true; break;}   //(2)
      case WLAN_STATUS_CONNECTED:    {stat = true; break;}   //(3)
    #else
      case WL_IDLE_STATUS:    {stat = false; break;} // 0
      #if WiFiType != AtEsp8266
        case WL_NO_SSID_AVAIL:  {stat = false; break;} // 1
        case WL_SCAN_COMPLETED: {stat = false; break;} // 2
        case WL_CONNECTION_LOST:{stat = false; break;} // 5
      #endif
      case WL_CONNECTED:      {stat = true;  break;} // 3
      case WL_CONNECT_FAILED: {stat = false; break;} // 4
      case WL_DISCONNECTED:   {stat = false; break;} // 6
      #if BoardType < Esp8266 
        case WL_AP_LISTENING:   {stat = true;  break;} // 7
        case WL_AP_CONNECTED:   {stat = true;  break;} // 8
        case WL_AP_FAILED:      {stat = false; break;} // 9
      #endif  
      case WL_NO_SHIELD:      {stat = false; break;} // 255
    #endif
    default:                {stat = false; break;}
  }
  #endif //oldversion
  { 
   #if WiFiType == AtEsp8266  
     case WL_NO_SHIELD      :{stat = false; break;}
     case WL_IDLE_STATUS    :{stat = false; break;}
     case WL_CONNECTED      :{stat = true;  break;}
     case WL_CONNECT_FAILED :(stat = false; break;}
     case WL_DISCONNECTED   :{stat = false; break;}
   #elif WiFiType == Cc3000  
     case WLAN_STATUS_DISCONNECTED :{stat = false; break;}
     case WLAN_STATUS_SCANNING     :{stat = false; break;}
     case WLAN_STATUS_CONNECTING   :{stat = true;  break;}
     case WLAN_STATUS_CONNECTED    :{stat = true;  break;}
   #elif WiFiType == EspNow //{stat = true;  break;}
   #else // all the rest use same standard flags
    case WL_NO_SHIELD       :{stat = false; break;}  
    case WL_IDLE_STATUS     :{stat = false; break;}
    case WL_NO_SSID_AVAIL   :{stat = false; break;}
    case WL_SCAN_COMPLETED  :{stat = false; break;}
    case WL_CONNECTED       :{stat = true;  break;}
    case WL_CONNECT_FAILED  :{stat = false; break;}
    case WL_CONNECTION_LOST :{stat = false; break;}
    case WL_DISCONNECTED    :{stat = false; break;}
#if defined WL_AP_LISTENING
    case WL_AP_LISTENING    :{stat = false; break;}
    case WL_AP_CONNECTED    :{stat = true;  break;}
    case WL_AP_FAILED       :{stat = false; break;}
#endif
   #endif
    default:                {stat = false; break;}
  }
//  Dbg("Wifi Connected",stat,0); Dbg(" Status",wstatus,1);
  return stat;
}

void WiFiFlashSettings(int mode) //0 does nothing
{
  #if IsEsp > 0  // only applies to esp boards
    // Dbg("WiFi Flash","Begin",0);
    if (mode > 0)
    {
      if (mode > 3) WiFi.softAPdisconnect(); //4
      if (mode > 2) WiFi.setAutoConnect(0);  //3 
      if (mode > 1) WiFi.persistent(false);  //2
      if (mode > 0) WiFi.disconnect();       //1
    }
    else if (mode < 0)
    {
      if (mode < -1) WiFi.setAutoConnect(1);  //-2 
      if (mode < 0)  WiFi.persistent(true);   //-1
    }
    //Dbg("  WiFi Flash","End",1);
  #elif WiFiType == cC3000
    if (mode == 3) WiFi.deleteProfiles();
  #endif
}

void DispMacId ()
{
 // Dbg("Find","Mac Address",1);
  #if WiFiType == MkrWiFi101
    return ;
  #endif
  #if WiFiType == Cc3000
    char Mac[17];  
    byte mac[6]={0,0,0,0,0,0};
    WiFi.getMacAddress(mac);
    sprintf(Mac,"%X:%X:%X:%X:%X:%X",mac[5],mac[4],mac[3],mac[2],mac[1],mac[0]);
  #elif (WiFiType != AtEsp8266 && WiFiType != AirLift && WiFiType != MkrWiFi101)
    String Mac;
    Mac = WiFi.macAddress();  
  #else
    char Mac[17];  
    byte mac[6]={0,0,0,0,0,0};
    WiFi.macAddress(mac);
    sprintf(Mac,"%X:%X:%X:%X:%X:%X",mac[5],mac[4],mac[3],mac[2],mac[1],mac[0]);
  #endif
  Dbg("Mac Address",Mac,1);
}

void WiFiReset()
{
//   WiFi.reset();
}

bool SetupWifi(char* ssid[],char* pass[],int Tries)
{  
  Dbg("WiFi Setup","Begin",1);
  int x = 0;
   
  #if WiFiType == AtEsp8266  // initialize serial for ESP module
    EspSerial.begin(WiFiBaud);
    while (!EspSerial);
 //   WiFiReset(); 
    WiFi.init(&EspSerial);
    if (WiFi.status() == WL_NO_SHIELD)
       FatalError("No Esp WiFi",WiFiErrorLed);
    else
       Dbg("Esp WiFi OK @",WiFiBaud,1);
  #elif WiFiType == AirLift
    #ifndef UseGenericNina 
      WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
    #endif
    if (WiFi.status() == WL_NO_MODULE) 
       FatalError("No AirLift Module",WiFiErrorLed);
    else 
    {
       Dbg("AirLift @Pin",SPIWIFI_SS,1);
       Dbg("Using NiNa V",WiFi.firmwareVersion(),1);
    //  WiFi.noLowPowerMode();
    //  WiFi.disconnect();
    //  WiFi.end();
    //  WiFi.channel(1);
    }
  #elif WiFiType == SpiEsp8266
       WiFi.init(WiFiPin); 
      //           uint32_t max_speed = 0, 
      //           SPIClass *in_spi = &SPI,
      //           int8_t hwResetPin = -1);
  #elif WiFiType == Cc3000
     if (!WiFi.begin()) 
     FatalError("No WiF Module",WiFiErrorLed);
  #endif
  #if defined WiFiSetFlash 
    WiFiFlashSettings(WiFiSetFlash); 
  #endif

  DispMacId();

  if (TryAps > 0)
  {
    while (x < TryAps)
    {
      Dbg("Add NetWork",ssid[x],1);
      #if WiFiType != Cc3000
        wifiAutoSelector.add(ssid[x],pass[x]);
      #endif
      x++;
    }
    
    if (NetworkConnect(Tries)) return true;  
    if (AutoAp < 0)
    {
      Dbg("Wifi Networks","Not Found",1);
      return false;  
    }
    else
    {
      Dbg("Wifi Networks","Not Found So..",1);
      if (!StartAP(ssid[AutoAp],pass[AutoAp])) FatalError("Cannot Start AP",WiFiErrorLed);
      else return true;  
    }
  }
  else  // just start AP 
  {  
    x = AutoAp;
    if (AutoAp < 0) x = 0;
    if (!StartAP(ssid[x],pass[x])) FatalError("Cannot Start AP",WiFiErrorLed);
    else return true;
  }
  // stop stupid compiler warnings.
  return false;
}
#endif