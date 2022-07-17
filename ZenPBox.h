#ifndef ZenPBox
 #define ZenPBox

#if defined PBScenario
//Pushingbox Details
//#define PBScenario  "vB8DFF6A221A7279"  //define in prog 
#define PBHost "api.pushingbox.com"

void SendToPb(String Scenarioid,String VarName[], String Data[])
{
//http://api.pushingbox.com/pushingbox?devid=vB8DFF6A221A7279&humidityData=33&celData=44&fehrData=111&hicData=22&hifData=77
  String DataString;
  int x = 0;
  while(true)
  { 
    if (VarName[x].length() == 0) break;
    DataString  += "&";
    DataString  += VarName[x]; 
    DataString  += "=";
    DataString  += Data[x];
    x++;
  }
  Dbg("Data",DataString,1);
  
  //only allowed 100 a day
  Dbg("Sending to Server","",1); 
  WiFiClient client;  
  if (client.connect(PBHost, 80))
  { 
    String Command = "GET /pushingbox?devid=" + Scenarioid + DataString;
    Dbg("Command",Command,1); 
    client.print(DataString);
    client.println(" HTTP/1.1"); 
    client.print("Host: ");
    client.println(PBHost);
    client.println("User-Agent: Zen/1.0");
    client.println();
    Dbg("Data","Sent",1); 
  }
  else Dbg("No Connection","",1);
}
#endif
#endif