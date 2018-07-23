/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org/>
 */
#include <Wire.h>    
/*
Board         I2C / TWI pins
Uno, Ethernet A4 (SDA), A5 (SCL)
Mega2560      20 (SDA), 21 (SCL)
Leonardo      2 (SDA), 3 (SCL)
Due           20 (SDA), 21 (SCL), SDA1, SCL1
 */
#define disk1 0x50    //Address of 24LC256 eeprom chip

static void testOk() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

static void testFail() {
  for (uint8_t i=0; i<3; ++i) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}

void log(const char *str) {
  div_t d = div(millis(), 1000);
  Serial.print(d.quot), Serial.write('.'), Serial.print(d.rem);
  Serial.print(" >>> ");
  Serial.println(str);
}

static volatile uint8_t *extmem = reinterpret_cast<volatile uint8_t*>(0x8000);

bool test() {
  for (uint16_t i = 0; i<32768; ++i) {
    extmem[i] = (i+1) & 0xFF;
  }
  for (uint16_t i = 0; i<32768; ++i) {
    if (extmem[i] != ((i+1) & 0xFF))
      return false;
  }
  return true;
}

void writeEEPROM(int deviceaddress, uint16_t eeaddress, uint8_t data ) 
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
 
  delay(5);
}

void writeEEPROM16(int deviceaddress, uint16_t eeaddress ) 
{
  uint8_t i;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  for (i=0;i<16;i++) Wire.write(extmem[eeaddress+i]);
  Wire.endTransmission();
 
  delay(6);
}
 
uint8_t readEEPROM(int deviceaddress, uint16_t eeaddress ) 
{
  byte rdata = 0xFF;
 
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
 
  Wire.requestFrom(deviceaddress,1);
 
  if (Wire.available()) rdata = Wire.read();
 
  return rdata;
}

void setup() {
  long startTime, stopTime;
  bool res;
  uint16_t address, count;

  Wire.begin();  

  auto doTest = [&]() {
    delay(1000);
    startTime = millis();
    res = test();
    stopTime = millis();
    if (res) {
      log("PASSED, time (ms):"), Serial.write('\t'), Serial.println(stopTime-startTime);
      testOk();
    } else {
      log("FAILED");
      testFail();
    }
  };

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  log("Enabling external memory interface");
  bitSet(XMCRA, SRE);  // enable externalmemory
  bitSet(XMCRB, XMM0); // release unused pin PC7
  
  log("1. Wait cycles: 2 + 1");
  bitSet(XMCRA, SRW11);
  bitSet(XMCRA, SRW10);
  doTest();

  log("2. Wait cycles: 2");
  bitSet(XMCRA, SRW11);
  bitClear(XMCRA, SRW10);
  doTest();

  log("3. Wait cycles: 1");
  bitClear(XMCRA, SRW11);
  bitSet(XMCRA, SRW10);
  doTest();

  log("4. Wait cycles: 0");
  bitClear(XMCRA, SRW11);
  bitClear(XMCRA, SRW10);
  doTest();

  //-------------
  log("EEprom write - 32768/16 16 bytes write");
  //fill extmem
  for (uint16_t i = 0; i<32768; ++i) {
    extmem[i] = (i+1) & 0xFF;
    //writeEEPROM(disk1, i, (i+1) & 0xFF);
  }
  startTime = millis();
  for (uint16_t i = 0; i<32768; i+=16) {
    writeEEPROM16(disk1, i);
  }
  stopTime = millis();
  log("32768 single bytes written - time (ms):"), Serial.write('\t'), Serial.println(stopTime-startTime);

  log("EEprom read - 32768 single bytes");
  count = 0;
  startTime = millis();
  for (uint16_t i = 0; i<32768; ++i) {
    if (readEEPROM(disk1, i ) != ((i+1) & 0xFF)) count++;
  }
  stopTime = millis();
  log("32768 single bytes read - time (ms):"), Serial.write('\t'), Serial.println(stopTime-startTime);
  log("Errors - ");Serial.println(count);
  log("Finished");
}

void loop() {
  /* nothing to do
  int i;
  extmem[0]=1;extmem[1]=2;extmem[2]=3;extmem[3]=4;extmem[4]=5;
  for (i=0; i<5; i++) Serial.print(extmem[i]);
  Serial.println();
  */
}

