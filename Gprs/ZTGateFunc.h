// timeaalarm timers
AlarmId OpenTimer  = dtINVALID_ALARM_ID;
AlarmId CloseTimer = dtINVALID_ALARM_ID;
AlarmId SleepTimer = dtINVALID_ALARM_ID;
AlarmId WakeTimer  = dtINVALID_ALARM_ID;

#ifndef StayOpenLed
  #define StayOpenLed   YellowLed  // ztgatefunc.h
#endif

void Close();

void SetStatus(String Status)
{
 GateStatus = Status;
}

void CheckAutoclose()
{
  if ((AutoClose > 0) && (CloseTimer == dtINVALID_ALARM_ID))
    CloseTimer = Alarm.timerOnce(AutoClose, Close);
}

bool CheckGateStatus()
{
/*
  if (digitalRead(IsOpen)) 
  {
    SetStatus("Open");
    if (StayOpen) digitalWrite(StayOpenLed, HIGH);
    else CheckAutoclose();
    return true;
  } 
  else if (digitalRead(IsClosed))
  {
      //if ((StayOpen) Open() return;
      SetStatus("Closed");
      digitalWrite(StayOpenLed, LOW);
      StayOpen = 0;
      return false; 
  }
  else 
  {  // we dont know?!
     return false;
  }
*/
//  Dbg("Status",GateStatus,1);
}

void SetRelay(unsigned int Relay)
{
 // if (StayOpen and Relay == GateClose) return; 
  if (Relay == GateOpen)  Command = "Open"; 
  if (Relay == GateClose) Command = "Close";
  LogIt("","Gate Action",Command,User,Phone);
  digitalWrite(Relay, LOW);
  delay(2000);
  digitalWrite(Relay, HIGH);
  delay(2000);
  digitalWrite(Relay, LOW);
  delay(2000);
//  CheckGateStatus();
//  Command = "None";
}


void SetPower(bool Cmd)
{  // does nothing as inverter pin clashes with sdcard
  Dbg(F("Power"), Cmd, 1);
  if (!Cmd)
  {
 //   digitalWrite(Inverter, HIGH);
    SetStatus( "Asleep");
  }
  else
  {
 //   digitalWrite(Inverter, LOW);
    SetStatus( "Awake");
  }
}

void EndAlarm(AlarmId t)
{
  Alarm.free(t);
  // optional, but safest to "forget" the ID after memory recycled
  t = dtINVALID_ALARM_ID;
}

void Flasher()
{
  //digitalWrite(OpenLed, !digitalRead(OpenLed));
}

void Sleep()
{
  Dbg(F("Power"), F("Sleep"), 1);
  SetPower(0);
  EndAlarm(SleepTimer);
}

void Wake()
{
  Dbg(F("Power"), F("WAKE"), 1);
  SetPower(1);
  EndAlarm(WakeTimer);
}

void Close()
{
  Dbg(F("Gate"), F("CLOSE"), 1);
  SetRelay(GateClose);
  EndAlarm(CloseTimer);
}

void Open()
{
  Dbg(F("Gate"), F("OPEN"), 1);
  SetRelay(GateOpen);
  EndAlarm(OpenTimer);
  CheckAutoclose();
}

int GetHr(String param)
{
  return atoi(GetElement(param, 0, '#').c_str());
}

int GetMn(String param)
{
  return atoi(GetElement(param, 1, '#').c_str());
}

void ProcessCall()
{
  #ifdef NoSim
     return;
  #endif
  Open();
  CloseTimer = Alarm.timerOnce(20, Close);
}

void ProcessCommand()
{
  String ValidCommands[] = {"open","close","oc","add","delete","openfor","reboot","autoclose","openin","openat","opentill","closeat","openfor","recharge","balance","sleepfor","sleeptill","wakeat","remote","sleep","wake","status","users","help","load","clear","\0"};
 // LogIt("",Command,Param,User,Phone);
  switch(GetIndexOf(ValidCommands,Command))
  {
    case -1:  // invalid
    {
      Dbg("Invalid Command",Command,1);
      break;
    }
    case 0:  // open
    {
      if (!IsNumeric(Param))
      {
        Dbg("Param", Param, 1);
        OpenBy = Phone;
        Dbg("OpenBy", OpenBy, 1);
        StayOpen = 1;
        EndAlarm(CloseTimer);
      }
      else if (Param == "2000") CloseTimer = Alarm.timerOnce(atoi(Param.c_str()) * 60, Close);
           else CheckAutoclose();

      Open();
      break;
    }
    case 1:  // close
    {
      Dbg("Stayopen", StayOpen, 1);
      Dbg("Phone", Phone, 1);
      Dbg("OpenBy", OpenBy, 1);
      if (!StayOpen) Close();
      else if (Param == "override") Close();
      else if (IsAdmin(User)) Close();
      else if (Phone == OpenBy) Close();
      else {
      //            char mes[50] = "Sorry Gate Can Only Be Closed By : ";
      //            strcat(mes,OpenBy);
      //            GprsSendSms(Phone.c_str(), mes);
      }
      break;
    }
    case 2:  // oc
    {
      ProcessCall(); // same just ringing number 
      break;
    } 
    case 3:  // add
    {
      String number;
      String name;
      if (Param.indexOf(DelimTwo) > 0) 
      {
        number = GetElement(Param,0,DelimTwo);
        name   = GetElement(Param,1,DelimTwo);
      }
      else 
      {
        number = Param;
        name   = Param;
      }
      AddUser(number,name);
      break;
    }
    case 4:  // delete
    {
      DeleteUser(Param);
      break;
    }
    case 6:  // reboot
    {
      ReBootMe();
      break;
    }
    case 7:  // autoclose
    {
      AutoClose = atoi(Param.c_str()) * 60;
      if (AutoClose == 0) EndAlarm(CloseTimer);
      break;
    }
    case 8:  // openin
    {
      OpenTimer = Alarm.timerOnce(atoi(Param.c_str()) * 60, Open);
      break;
    }
    case 9:  // openat
    {
      OpenTimer = Alarm.timerOnce(GetHr(Param), GetMn(Param), 0,  Open);
      break;
    }
    case 10:  // opentill
    {
      Open();
      CloseTimer = Alarm.timerOnce(GetHr(Param), GetMn(Param), 0,  Close);
      break;
    }
    case 11:  // closeat
    {
      CloseTimer = Alarm.timerOnce(GetHr(Param), GetMn(Param), 0,  Close);
      break;
    }
    case 12:  // openfor
    {
      Open();
      if (Param == "2000")
      {
        CloseTimer = Alarm.timerOnce(atoi(Param.c_str()) * 60, Close);
      }
      else CheckAutoclose();
      break;
    }
    case 13:  //recharge
    {
//      GprsSendSms("132", Param.c_str());
      break;
    }
    case 14:  // balance  // redundant
    {
//      GprsSendSms("131", "abc"); // need to waitforreply
      break;
    }
    case 15:  // sleepfor  //redundant
    {
     // SetPower(0);
     // WakeTimer = Alarm.timerOnce(atoi(Param.c_str()) * 60, Wake);
      break;
    }
    case 16:  // sleeptill redundant
    {
//     SetPower(0);
//     WakeTimer = Alarm.alarmOnce(GetHr(Param), GetMn(Param), 0,  Wake);
      break;
    }
    case 17:  // wakeat  //redundant
    {
//     WakeTimer = Alarm.alarmOnce(GetHr(Param), GetMn(Param), 0,  Wake);
      break;
    }
    case 18:  // remote   // redundant
    {
//      SetRelay(RemotePower); // bounce the remote power
      break;
    }
    case 19:  // sleep  // redundant
    {
//      SetPower(0);
      break;
    }
    case 20:  // wake   // redundant
    {
//      SetPower(1);
      break;
    }
    case 21:  // status  // redundant
    {
//      GprsSendSms(Phone.c_str(), GateStatus.c_str());
      break;
    }
    case 22:  // users dump to userdump.txt
    {
      ExportUsers();
      break;
    }
    case 23:  // help  // redundant
    {
//      GprsSendSms(Phone.c_str(), "Open,Close,OC");
      break;
    }
    case 24:  // Load Users from users.txt
    {
      LoadUsers(ToBool(Param));
    }
    case 25:   // clear users
    {
      ClearUsers();
    }
    default:  //
    {
      break;
    }
  }
}

void ProcessSms(int SmsId)
{
  #ifdef NoSim
     return;
  #endif
  if (SmsId = 0) return;

  // retrieve and chop up the Sms into Command Param User Phone
  if (SmsId > 0) if (!GetMessage(SmsId)) return;
  // oterwise we alrady got details in Extractcommand
  //then ok to proceed.    
  ProcessCommand();
}

