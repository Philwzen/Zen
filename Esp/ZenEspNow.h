#ifndef ZenEspNow
#define ZenEspNow
#if WiFiType == EspNow

const uint8_t *peer_addr;
esp_now_peer_info_t slave;

// Global copy of slave
//#define NUMSLAVES 20
//esp_now_peer_info_t slaves[NUMSLAVES] = {};
//unsigned int SlaveCnt = 0;
unsigned int CHANNEL = 0;
//#define DispResult

void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Dbg("ESPNow Init","Success",1);
  }
  else {
    Dbg("ESPNow Init","Failed",1);
    ESP.restart();
  }
}

void DisplayResult(esp_err_t param)
{
  #ifdef DispResult
   if (param == ESP_OK) {
      Dbg("Success","",1);
    } else if (param == ESP_ERR_ESPNOW_NOT_INIT) {
      Dbg("ESPNOW not Init.","",1);
    } else if (param == ESP_ERR_ESPNOW_ARG) {
      Dbg("Invalid Argument","",1);
    } else if (param == ESP_ERR_ESPNOW_INTERNAL) {
      Dbg("Internal Error","",1);
    } else if (param == ESP_ERR_ESPNOW_NO_MEM) {
      Dbg("ESP_ERR_ESPNOW_NO_MEM","",1);
    } else if (param == ESP_ERR_ESPNOW_NOT_FOUND) {
      Dbg("Peer not found.","",1);
    } else if (param == ESP_ERR_ESPNOW_FULL) {
      Dbg("Peer list full","",1);
    } else if (param == ESP_ERR_ESPNOW_EXIST) {
      Dbg("Peer Exists","",1);
    } else {
      Dbg("Not sure what happened","",1);
    }
 #endif
}

void CreateBroadcastSlave() {
  // clear slave data
  memset(&slave, 0, sizeof(slave));
  for (int ii = 0; ii < 6; ++ii) {
    slave.peer_addr[ii] = (uint8_t)0xff;
  }
  slave.channel = CHANNEL; // pick a channel
  slave.encrypt = 0; // no encryption 
  DisplayResult(esp_now_add_peer(&slave));
  peer_addr = slave.peer_addr;
}

void SetUpEspNow()
{
  CHANNEL = dipswitch() + 1;
  disableCore0WDT();
  disableCore1WDT();

  #if StationType == Transmitter
    Dbg("Using ssid",ssid,0);
    WiFi.StationType(WIFI_STA);
    InitESPNow();
    Dbg("Adding Broadcast","",1);
    CreateBroadcastSlave();
  #else  //receiver
    if (!WiFi.softAP(SSID.c_str(),"", CHANNEL, 0))
      Dbg("AP Config","Failed",1);
    InitESPNow();
    esp_now_register_recv_cb(OnDataRecv);
    Dbg("Listening As",SSID,0);
    Dbg(" On channel",CHANNEL,1);
  #endif
}

/*****
// Scan for slaves in AP StationType
void IRAM_ATTR ScanForSlave(char* ssid) {
  int8_t scanResults = WiFi.scanNetworks();
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  if (scanResults == 0) {
    Dbg("Nothing found","",1);
  } else {
    for (int i = 0; i < scanResults; ++i) {
      String SSID = WiFi.SSID(i);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf(String(ssid)) == 0) {  // "Slave"
        Dbg("Found",SSID,1);
        String BSSIDstr = WiFi.BSSIDstr(i);
        // Get BSSID => Mac Address of the Slave
        int mac[6];
	Dbg("BSSID",BSSIDstr,1);
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
        Dbg("using",BSSIDstr,1);
	  for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
	    Dbg("Slave",slaves[SlaveCnt].peer_addr[ii],1);
          }
        }
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
	Dbg("SlaveCnt",SlaveCnt,1);
      }
    }
  }
  // clean up ram
  WiFi.scanDelete();
}

void PairDevices() {
  if (SlaveCnt > 0) 
  {
    for (int i = 0; i < SlaveCnt; i++) 
    {
      if (esp_now_is_peer_exist(slaves[i].peer_addr))
           Dbg("Already Paired","",1);
      else DisplayResult(esp_now_add_peer(&slaves[i]));
    }
  } 
  else Dbg("No Slave found to process","",1);
}

****/
#endif
#endif