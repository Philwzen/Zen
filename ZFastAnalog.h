#ifndef ZFastanalog
#define ZFastAnalog

int inline analogReadFast(byte ADCpin, byte prescalerBits=4);
// prescaler = 2 = 12us default is 4 = 20us
// but not defined(TEENSYDUINO) 

#if defined(TEENSYDUINO)
  int inline analogReadFast(byte ADCpin, byte prescalerBits) // inline library functions must be in header
  {
    return analogRead(ADCpin);
  } 
#elif defined(__arm__) 
  int inline analogReadFast(byte ADCpin, byte prescalerBits) // inline library functions must be in header
  {
    ADC->CTRLA.bit.ENABLE = 0;                     // Disable ADC
    while( ADC->STATUS.bit.SYNCBUSY == 1 );        // Wait for synchronization
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV64 |   // Divide Clock by 64
                     ADC_CTRLB_RESSEL_10BIT; 
    ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |   // 1 sample 
                       ADC_AVGCTRL_ADJRES(0x00ul); // Adjusting result by 0
    ADC->SAMPCTRL.reg = 0x00;                      // Sampling Time Length = 0

    ADC->CTRLA.bit.ENABLE = 1;                     // Enable ADC
    while( ADC->STATUS.bit.SYNCBUSY == 1 );        // Wait for synchronization
    return analogRead(ADCpin);
  }
#else
  int inline analogReadFast(byte ADCpin, byte prescalerBits) // inline library functions must be in header
  {
    byte ADCSRAoriginal = ADCSRA; 
    ADCSRA = (ADCSRA & B11111000) | prescalerBits; 
    int adc = analogRead(ADCpin);  
    ADCSRA = ADCSRAoriginal;
    return adc; 
  }
#endif

#endif