#ifndef ZenWiFiAutoSelect
#define ZenWiFiAutoSelect

class WiFiAutoSelector {
private:

  typedef struct _NetworkInfo {
    int  rssi;
    char ssid[32];
    char key[64];
  } NetworkInfo;

protected:

  NetworkInfo* _networks;
  int          _count;
  int          _capacity;
  int          _connectedIndex;
  int          _connectTimeout;
  
  bool resize(int newCapacity) {
    if(newCapacity <= _capacity) return true;
    NetworkInfo* tni = (NetworkInfo*)malloc( sizeof(NetworkInfo) * newCapacity );
    if(!tni) return false;
    memset(tni, 0, sizeof(NetworkInfo) * newCapacity);
    for(int i=0; i < _count; i++) {
      tni[i] = _networks[i];
    }
    if(_networks) free(_networks);
    _networks = tni;
    _capacity = newCapacity;
    return true;
  }
  
public:
  int getCount() { return _count; }
  int getCapacity() { return _capacity; }
  int getConnectedIndex() { return _connectedIndex; }
  
  int getRSSI(int index) { return _networks[index].rssi; }
  char* getSSID(int index) { return _networks[index].ssid; }
  char* getKey(int index) { return _networks[index].key; }
  
  void clear() {
     memset(_networks, 0, sizeof(NetworkInfo) * _capacity );
     _count = 0;
  }

  int add(const char* ssid, const char* key) {
    bool canAdd = true;
    if((_count + 1) >= _capacity) { canAdd = resize( _capacity + 4 ); }
    if(canAdd) {
      _networks[_count].rssi = -1000;
      strncpy(_networks[_count].ssid, ssid, sizeof(_networks[_count].ssid));
      strncpy(_networks[_count].key, key, sizeof(_networks[_count].key));
      _count++;
      return _count - 1;    
    }
    return -1;
  }

  int scanAndConnect() {
    int bestNetworkIndex = UseNetId; // -1 to autoselect
    if (bestNetworkIndex < 0)
    {
      bestNetworkIndex = -1;
      _connectedIndex = -1;
      if(!_networks || !_count) return _connectedIndex;
      for(int i = 0; i < _count; i++) {
        _networks[i].rssi = -1000;
      }
      int foundNetworkCount = WiFi.scanNetworks();
      if(0 >= foundNetworkCount) return _connectedIndex;
      int bestRSSI = -1000;
      
      while(foundNetworkCount-- > 0 ) 
      {    
        String foundSSID = WiFi.SSID(foundNetworkCount);
        for(int i = 0; i < _count; i++ ) 
        {
          if(foundSSID.equals(_networks[i].ssid)) 
          {
            _networks[i].rssi = WiFi.RSSI(foundNetworkCount);
            #if defined Zen
              Dbg(F("Found"),F(""),0);
	      Dbg(foundSSID,_networks[i].rssi,1);
            #endif
            // Keep the network index with the best signal strength
            if(bestRSSI < _networks[i].rssi) 
            {
              bestRSSI = _networks[i].rssi;
              bestNetworkIndex = i;
            }                  
            break;         
          }        
        }
      } 
    }
    // If we have found th best network, connect to it.
    if(-1 < bestNetworkIndex) {
      Dbg(F("Connecting"),_networks[bestNetworkIndex].ssid,1);
      KickDog();
      WiFi.begin(_networks[bestNetworkIndex].ssid, _networks[bestNetworkIndex].key);
      // Wait for the wifi to connect. This should happen with in the timeout period.
      for(int loop = _connectTimeout; loop > 0; loop -= 200 ) {
        delay(200);
        if(WiFi.status() == WL_CONNECTED) {
          _connectedIndex = bestNetworkIndex;      
          break;
        }
      }
    }
    return _connectedIndex;
  }

  WiFiAutoSelector() {
    if(_networks) free(_networks);
  }

  WiFiAutoSelector(int connectTimeout) : 
    _networks(0L), _count(0), _capacity(0)
  , _connectedIndex(-1) , _connectTimeout(connectTimeout) {
  }
  
};
#endif