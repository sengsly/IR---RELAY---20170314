#include <EEPROM.h>
#define PCINT_VERSION 1
#define digitalPinToInterrupt(P) ( NOT_AN_INTERRUPT )

#include <IRLremote.h>


    //if (digitalPinToInterrupt(pin) != NOT_AN_INTERRUPT){

//Update 2017 04 17 
//Adding reliable remote control protocol library


#define IRPin     0
#define LEDPin    2
#define RELAY    1


/*
#define IRPin     2
#define LEDPin    13
#define RELAY    6
*/



#define REPEATCODE 5
#define ADDRESS   0
#define MAXINPUT  5000    //max 5s for the input



CHashIR IRLremote;

uint16_t currentpulse = 0; // index for pulses we're storing
uint16_t numpulse = 0;
byte pressed=0;

uint32_t irCode = 0;
uint32_t lastIRCode = 0;
uint32_t EEPROM_IRCode = 0;
unsigned long timelapse;
unsigned long timelapseNew;


bool TURN_ON=0;

void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte e4 = (value & 0xFF);
  byte e3 = ((value >> 8) & 0xFF);
  byte e2 = ((value >> 16) & 0xFF);
  byte e1 = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, e4);
  EEPROM.write(address + 1, e3);
  EEPROM.write(address + 2, e2);
  EEPROM.write(address + 3, e1);
}

uint32_t EEPROMReadlong(long address) {
  uint32_t e4 = EEPROM.read(address);
  uint32_t e3 = EEPROM.read(address + 1);
  uint32_t e2 = EEPROM.read(address + 2);
  uint32_t e1 = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((e4 << 0) & 0xFF) + ((e3 << 8) & 0xFFFF) + ((e2 << 16) & 0xFFFFFF) + ((e1 << 24) & 0xFFFFFFFF);
}


void setup(void) {
//  Serial.begin(9600);
  
//    Serial.print("Digital pin is ");
//    Serial.println(digitalPinToPCINT(IRPin), DEC);

  pinMode(IRPin, INPUT);   // IR Pin
  pinMode(LEDPin, OUTPUT);   //
  pinMode(RELAY, OUTPUT);   //

  delay(3000);                  // waits for 5 seconds

  //flashing indicate running
  flashingLED(5);
  digitalWrite(LEDPin,HIGH);    //Turn on indicate running
  EEPROM_IRCode = EEPROMReadlong(ADDRESS);
   IRLremote.begin(IRPin);

}

void loop(void) {
    if (IRLremote.available())
    {
 
      auto data = IRLremote.read();
      irCode=data.command;
        
//      Serial.print("The code is");
//      Serial.println(irCode, HEX); // print lower 16 bits in hex
        
      if(irCode==EEPROM_IRCode){
    
      //    Serial.println("The code match");
          if (!TURN_ON){
            TURN_ON=true;
            digitalWrite(RELAY, HIGH);
          }else{
            TURN_ON=false;
            digitalWrite(RELAY, LOW);
          }
          flashingLED(2);
          delay(300);
      }else{
        if( lastIRCode == irCode){
            pressed++;
            if(pressed>=REPEATCODE){
              
              if((millis()-timelapse)<=MAXINPUT){
        //        Serial.println("New code matching");
                EEPROM_IRCode=irCode;
                EEPROMWritelong(ADDRESS,EEPROM_IRCode);
                flashingLED(3);
                delay(500);
              }
              timelapse=millis();
              pressed=0;      
            }
        }else{
            timelapse=millis();
            pressed=0;      
        }
        delay(50);
      }
      lastIRCode=irCode;
    }
}

void flashingLED(int count){
  for(int i=0;i<count;i++){
    digitalWrite(LEDPin,LOW);
    delay(250);                 
    digitalWrite(LEDPin,HIGH);
    delay(250);                 
  }
}
/*
void blinkLed(void) {
  // Switch Led state
  digitalWrite(LEDPin, !digitalRead(LEDPin));
}
*/
