


//Create Address from DIP Switch (4 positions used)
byte dipswitch() {
  int j = 0;
  byte addresses[][6] = {"0Laser,1Laser","2Laser","3Laser","4Laser"};
/**********************
  int dipPins[] = {2, 3, 4, 5}; //DIP Switch Pins
  for (int i = 0; i <= 3; i++) {
    pinMode(dipPins[i], INPUT);      // sets the digital pin 2-5 as input
    digitalWrite(dipPins[i], HIGH); //Set pullup resistor on
    2000);
    j = (j << 1) | digitalRead(dipPins[i]);   // read the input pin
  }
**********************/
  return addresses[j]; //return address
}

bool Rf24Setup(RF24 radio,char* mode,char* pwr){
  radio.begin();
  int Id;
  Dbg("Init", "OK",1);
  Id = dipswitch() + 1;
  byte addresses[][6] = {"0Node,1Node","2Node"};
// tran write on ID rec read on ID

  radio.setChannel(108);  //max 124
  radio.setRetries(0,0); // delay between retries & # of retries
  radio.enableDynamicAck();
  radio.setAutoAck(0);
  radio.setCRCLength(RF24_CRC_16);  
  radio.disableCRC();
  radio.setPayloadSize(IldaSize  * InDacRes);
  
  Dbg("Power",pwr,1);
  if (strcmp(pwr,"Min") == 0) {
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS); //RF24_1MBPS, RF24_2MBPS, RF24_250KBPS 
  }
  else {
   // radio.setAutoAck(true);          // autoACK if true receiver power needs to be max
    radio.setPALevel(RF24_PA_MAX);  
    radio.setDataRate(RF24_2MBPS); //RF24_1MBPS, RF24_2MBPS, RF24_250KBPS 
  }

  Dbg("Mode",mode,1);
  if (strcmp(mode,"Transmitter") == 0) {
	radio.openWritingPipe(addresses[Id]);
	radio.openReadingPipe(1,addresses[0]);
   	Dbg("Write On",Id,1);
	radio.stopListening();
  }
  else {  // its a receiver
	radio.openWritingPipe(addresses[0]);
	radio.openReadingPipe(1,addresses[Id]);
   	Dbg("Read On",Id,1);
     	radio.startListening();
  }
  return true;
}

