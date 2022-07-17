#ifndef dbg
 #define dbg
 int DBG = 1;
#else
 DBG++;
#endif

msg(F("DBG"),DBG,1);
