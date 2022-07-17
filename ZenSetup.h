#ifndef ZenSetup
  #define ZenSetup

  //Stuff that must be done in Setup
  char title[20];
  strncpy(title,IAm,20);

  ZenInit(DbgSerialBaud,title);

  Dbg(F("ZenSetup"),F("Done"),1);
#endif