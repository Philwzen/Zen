#ifndef ZenUser
#define ZenUser

bool IsAdmin(String user)
{ 
  bool ok = false;
  user.trim();
  ok = Policy.AdminList.indexOf(user) >= 0;  // -1 is not found
  if (!ok) 
    {
      Dbg(F("Not An Admin"),user,1);
     // Dbg(F("Admins"),Policy.AdminList,1);
     // LogIt("","Security","Not An Admin",user,"");
    }
  return ok;
}

bool SecurityEnabled()
{
  return Policy.UseSecurity;
}

#ifdef PPSecurity
// User Admin stuff
  bool GprsValidUser(String phone);

  String GetUser(String Key)
  {
    return (GetIniVar(UserFile,Key,DelimTwo));
  }
  
  bool UserExpired(String Key)
  {
    return false;
  }

  bool ValidUser(String Key)
  {
    if (SecurityEnabled())
    {
      bool result;
      Key.toLowerCase();
      if (IsAdmin(Key)) return true;

      #if FileSys > None
        result =  GetUser(Key) != "Not Found";
      #elif GprsPresent > None 
        result = GprsValidUser(Key.c_str());
      #else
        result =  true;
      #endif 

     // if (!result) LogIt("","Security","Rejected Invalid User",Key,"");
      return result;
    }
    else 
    {
      //LogIt("","Security","Disabled Unknown Allowed",Key,"");
      return true;
    }
  }

  bool DeleteUser(String Key)
  {
    Dbg("Delete User",Key,1);
    
    //= del logic
    if (SecurityEnabled())
    {
      Key.toLowerCase();
      #if FileSys > None
        return true;
      #elif GprsPresent > None
          return GprsDeleteUser(Key);    
      #else
        return true;
      #endif 
    }
    else return true; 
  }

  bool AddUser(String Key,String Name)
  {
      Key.toLowerCase();
      Name.toLowerCase();
      #if FileSys > None
        String Scratch = Key + DelimTwo + Name; 
        File fileOut = FileOpenWrite(UserFile); 
        if (!fileOut) return false;
        if (FileWriteLine(fileOut,Scratch.c_str()))
        {
          FileClose(fileOut); 
          return true;
         }
         else return false;
      #elif GprsPresent > None
        return GprsAddUser(Key,Name);
      #else
        return true;
      #endif 
  }
#else  // placeholders just incase
  String GetUser(String Key)
  {
    return "";
  }

  bool ValidUser(String Key)
  {
    Dbg("Validate",Key,1);
    LogIt("","Security Off","All Allowed",Key,"");
    Dbg("Security Off",Key,1);
    return true;
  }

  bool DeleteUser(String Key)
  {
    return true; 
  }

  bool AddUser(String Key,String Name)
  {
     return true;
  }
#endif
#endif
 