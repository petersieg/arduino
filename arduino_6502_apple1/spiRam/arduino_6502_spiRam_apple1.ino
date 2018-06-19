// This one is using spi ram 23LCV512 for ram memory.
// This appears under memtest factor 100-500 times slower!!
// In real world apps, like mandel3.bas app. 10-20 times slower.
// So it is at the etch to be usable.

#include <SPI.h>
#define SS0 24  // I am using a MEGA2560
#define MODE SPI_MODE0
#define BAUD 20000000L
#define FORMAT MSBFIRST
#define CHIP 512

// 12.01.2015 (c) P.Sieg simple Apple 1 emulator based on arduino_6502 
// project from miker00lz (Mike Chambers)
// Link: http://forum.arduino.cc/index.php?topic=193216.0
// apple 1 roms are (c) Steve Wozniak / Apple Inc. Status is Freeware
// according to: http://www.callapple.org/soft/ap1/emul.html
// a1 assembler (c) 9/2006 San Bergmans released as freeware
// Link: http://www.sbprojects.com
// My code released under GNU GPL V2


uint8_t curkey = 0;
uint8_t iskey  = 0;

extern "C" {
  uint16_t getpc();
  uint8_t getop();
  void exec6502(int32_t tickcount);
  void reset6502();
  void serout(uint8_t val) {
    //Serial.print(val, HEX);
    //Serial.println();
    if (val == 13) {
      Serial.println();
      }
    else {
      Serial.write(val);
      }  
  }
  uint8_t isakey() {
    if (Serial.available()) iskey  = 0x80;
    else iskey = 0;
    return(iskey);
  }
  uint8_t getkey() {
    curkey = Serial.read() & 0x7F;
    // make a-z => A-Z
    if ((curkey >= 97) && (curkey <= 122)) curkey = curkey - 0x20;
    return(curkey);
  }
  void clearkey() {
    curkey = 0;
  }
  void printhex(uint16_t val) {
    Serial.print(val, HEX);
    Serial.println();
  }
  void initRAM( uint8_t csPin) {
    SPI.beginTransaction(SPISettings(BAUD, FORMAT, MODE));
    digitalWrite(csPin,LOW); // select SPI Ram
    SPI.transfer(0x01); // write to Mode Register
    SPI.transfer(0x40); // set sequential mode
    digitalWrite(csPin,HIGH); // DeSelect SPI ram chip
    SPI.endTransaction();
  }
  void writeSPIByte(uint8_t csPin, uint16_t adr, uint8_t dataByte){
    SPI.beginTransaction(SPISettings(BAUD, FORMAT, MODE));
    digitalWrite(csPin,LOW); // select SPI Ram
    SPI.transfer(0x02); // write command
#if !(CHIP == 512)
    SPI.transfer((uint8_t)((adr>>16)&0xFF));
#endif
    SPI.transfer((uint8_t)((adr>>8)&0xFF));
    SPI.transfer((uint8_t)(adr&0xFF));
    SPI.transfer(dataByte);
    digitalWrite(csPin,HIGH); // DeSelect SPI ram chip
    SPI.endTransaction();
  }

  uint8_t readSPIByte(uint8_t csPin, uint16_t adr){
    SPI.beginTransaction(SPISettings(BAUD, FORMAT, MODE));
    digitalWrite(csPin,LOW); // select SPI Ram
    SPI.transfer(0x03); // read command
#if !(CHIP == 512)
    SPI.transfer((uint8_t)((adr>>16)&0xFF));
#endif
    SPI.transfer((uint8_t)((adr>>8)&0xFF));
    SPI.transfer((uint8_t)(adr&0xFF));
    uint8_t result = SPI.transfer(0);
    digitalWrite(csPin,HIGH); // DeSelect SPI ram chip
    SPI.endTransaction();
    return result;
  }
}

void setup () {
  // turn all CS pin off (high)
  digitalWrite(SS0,HIGH);
  // Drive CS pins
  pinMode(SS0,OUTPUT);
  // init SPI hardware
  SPI.begin();
  initRAM(SS0);
  Serial.begin (9600);
  Serial.println("apple 1 emulator");
  Serial.println ();

  reset6502();
}

void loop () {
  exec6502(100); //if timing is enabled, this value is in 6502 clock ticks. otherwise, simply instruction count.
}

