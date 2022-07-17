// Web Page related stuff
#if WiFiType > None
  #ifndef NoHttp

void GateRoot() 
{
   Dbg("Begin","GateRoot",1);
//   #if FileSys > None
     SrvSendFile(RootPage,"text/html");
//   #endif
   Dbg("Done","GateRoot",1);
}

void GateHandleCmd()
{
    User  = "Web";
    Phone = User;
    Param = "";

//  "open","close","oc","add","delete","reboot",
//   "autoclose","balance","remote","sleep",
//   "wake","status","users","help","load","clear","\0"

//  require param = something
//"openfor","openin","openat","opentill","closeat","openfor","recharge",
//"sleepfor","sleeptill","wakeat",


    Command = WebSrv.arg("cmd");
#ifdef ZGprs
    ProcessCommand();
#endif
}

void GateHandleForm()
{
  Dbg("Begin", "GateHandleForm", 1);
  String fname = WebSrv.arg("logs");
  fname.trim();
//  #if FileSys > None
    SrvSendFile(fname,"text/plain");
//  #endif 
  Dbg("Done", "GateHandleForm", 1);
}

void GateSendLogList()  // send list of log files
{
  Dbg("Begin", "GateSendLogList", 1);
  SrvSendJsonDirList("/",".log","Logs");
  Dbg("Done", "GateSendLogList", 1);
}
  #endif
#endif