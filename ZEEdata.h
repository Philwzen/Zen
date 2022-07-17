#ifndef ZEEdata
#define ZEEdata
#ifdef UseEEprom  // move to filesys.h ???
#include <EEPROMex.h>
const int maxAllowedWrites = 2000;
const int memBase          = 1;
const int eepos = 1;
const int eelen = UserSize;

void WriteEEData(int pos, char* ibuff, int len)
{
//  Dbg(F("WriteEEData"),ibuff,1);
  EEPROM.updateBlock<char>(pos, ibuff, len);
}

void ClearEEData()
{
 char x[eelen];
 for (int i = 0 ; i < eelen ; i++) {
    x[i] = (char)0;
  }
 EEPROM.updateBlock<char>(0, x, eelen);
 // WriteEEData(0,x,eelen);
 // Dbg("ClearEEData","Done",1);
}

void ReadEEData(int pos, int len, char* Obuff )
{
  EEPROM.readBlock<char>(pos, Obuff, len);
}
#endif
#endif