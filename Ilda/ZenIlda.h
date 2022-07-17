
// ADC Resolution
#define InDacRes 10
#define OutDacRes 10

unsigned int ID;

//Struct for data tansmission
struct IldaData{
  unsigned int id : 8;
  unsigned int xp : 8;
  unsigned int yp : 8;
  unsigned int rp : 8;
  unsigned int gp : 8;
  unsigned int bp : 8;
}SentRec,OutRec;

// prepros for analog input pins
#if StationType == Transmitter
  #if BoardType == TinyPico
    #define xpi 25
    #define ypi 26
    #define rpi 27 	
    #define gpi 15 	
    #define bpi 14 	
  #elif BoardType == Teensy4 // T4
    #define xpi  A3
    #define ypi  A4
    #define rpi  A0
    #define gpi  A1
    #define bpi  A2
  #elif BoardType == Mkr1000
    #define xpi  A0
    #define ypi  A1
    #define rpi  A2
    #define gpi  A3
    #define bpi  A4
  #endif
#endif

#if StationType == Receiver
  // prepros for analog output registers/pins
  #if Dac == No  // use on chip dac and pwm
    #if BoardType == TinyPico
       #define xpo 25
       #define ypo 26
       #define rpo 27 	
       #define gpo 15 	
       #define bpo 14 	
       #include <analogWrite.h>
     #else // using a Teensy
       #define xpo 5 
       #define ypo 6 	
       #define rpo 7 	
       #define gpo 8 	
       #define bpo 9	
    #endif
  #else   // External Dac 
    #define DacCs  10   // slaveselect pin
    // convert to binary for adrDAC address
    #define xpo 0 
    #define ypo 1 
    #define rpo 2
    #define gpo 3
    #define bpo 4

    void ToBin(char* bnum,unsigned int inp,int bsize)
    {  
     // bnum = output return pointer 
     // inp = integer to be converted to binary
     // bsize = number of zero filled digits required in bnum
     // eg inp = 12 bsize = 5 so bnum = 01100

      bsize++;
      unsigned int x = bsize - 1;
      unsigned int y = bsize - 2;
      for (unsigned int z = 0; z < x; z++) {
          bnum[y-z] = ((inp>>z) & 0x1) ? '1' : '0';
      }
    }
  #endif
#endif

void DisplayData(char From[12])
{
  Dbg(From,"",0);
  Dbg(" X+", OutRec.xp, 0); 
  Dbg(" Y+", OutRec.yp, 0);
  Dbg(" Red+", OutRec.rp, 0); 
  Dbg(" Green+", OutRec.gp, 0);
  Dbg(" Blue+", OutRec.bp, 1); 
}

void Analogconfig()
{
  #if StationType == Receiver
    #if Dac == No
      // setup analog op pins pwm freq and resolution 
      #define PwmFreq 312500 // 234375 for teensy4 at 10bit cpu  180 or 120Mhz
      // analogReference(DEFAULT);
      analogWriteResolution(OutDacRes); 
      // T4 pwm freq 234375 cpu freq 120 rc cutoff freq 5khz R=330 C= 0.1micF
      // TinyPico 8 bits, the maximal frequency is 312.5 kHz.
    
    //  analoqWriteFrequency(rpo,PwmFreq);
    //  analoqWriteFrequency(gpo,PwmFreq);
    //  analoqWriteFrequency(bpo,PwmFreq);
      pinMode(xpo,OUTPUT); // onchip dac
      pinMode(ypo,OUTPUT); // onchip dac2 
      pinMode(rpo,OUTPUT); // pwm
      pinMode(gpo,OUTPUT); // pwm
      pinMode(bpo,OUTPUT); // pwm
    #else
	// SetupDac
    #endif   
  #else  //  Transmitter
    #if BoardType == Teensy4 
       analogReadRes(InDacRes);
    #endif   // set ADC resolution to this many bits
   // analogReadAveraging(1);  stops esp32 create AP  // average this many readings
/*****
    xpi  A3 //18
    ypi  A4 //16
    rpi  A0 //14
    gpi  A1 //15
    bpi  A2 //16
*****/
    pinMode(xpi,INPUT); 
    pinMode(ypi,INPUT);  
    pinMode(rpi,INPUT);
    pinMode(gpi,INPUT);
    pinMode(bpi,INPUT);

    //if using adc.h Teensy3.2
    //adc->setReference(ADC_REFERENCE::REF_3V3,0);
    //adc->setReference(ADC_REFERENCE::REF_3V3,1);
    //adc->setResolution(12,0); // 0 to 1023
    //adc->setResolution(12,1); // 0 to 1023
    //adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED,0);
    //adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED,1);
    //adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED,0);  
    //adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED,1);  
  #endif
}

