// ID select pins 
#ifndef Id1
  #define Id1 3 
#endif
#ifndef Id2
  #define Id2 5
#endif

#ifndef SSid
 #define SSid ""
#endif
#ifndef Pass
  #define Pass "" 
#endif
#ifndef BROADCASTPORT
  #define BROADCASTPORT 8888
#endif
#ifndef BATCHSIZE
  #define BATCHSIZE 1
#endif

//char ssid[12] = SSid; // "" will connect to AP called Laserxxxx  
//char pass[12] = Pass; // "" no security
bool InAPMode = false;
IPAddress BroadcastIp(255,255,255,255);
IPAddress ApIp(192,168,4,1);
IPAddress GateWayIp(192,168,4,1);
IPAddress Subnet(255,255,255,0); 
bool HaveClients = false;
unsigned int BroadcastPort = BROADCASTPORT; 

unsigned int SentPoints;
unsigned int BatchSize = BATCHSIZE;
unsigned int BatchCounter;
unsigned int recsize;

//Create Address from switch
int dipswitch() {
  int j = 0;
  pinMode(Id1,INPUT_PULLUP);
  pinMode(Id2,INPUT_PULLUP);
  delayMicroseconds(10);
  if (!digitalRead(Id1) and !digitalRead(Id2)) j = 0;
  if ( digitalRead(Id1) and !digitalRead(Id2)) j = 1;
  if (!digitalRead(Id1) and  digitalRead(Id2)) j = 2;
//  Dbg("Dip",j,1);
  return j;
}

int GetSSID(char* ssid)
{
 int Id = dipswitch();
 snprintf(ssid, 10, "Laser%04u", Id);
 return Id;
}

