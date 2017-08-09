/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             readWriteString.ino                                                               |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 3.0.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  29.05.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program shows the method of reading a string from the console and saving it to flash memory                       |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
#include<SPIFlash.h>

uint32_t strAddr;

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#if defined (SIMBLEE)
#define BAUD_RATE 250000
#define RANDPIN 1
#else
#define BAUD_RATE 115200
#define RANDPIN A0
#endif

SPIFlash flash;

bool readSerialStr(String &inputStr);

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(BAUD_RATE);
#endif
#if defined (ARDUINO_SAMD_ZERO) || (__AVR_ATmega32U4__)
  while (!Serial) ; // Wait for Serial monitor to open
#endif

  flash.begin();

#if defined __AVR_ATtiny85__
  randomSeed(65535537);
#else
  randomSeed(analogRead(RANDPIN));
#endif
  strAddr = random(0, flash.getCapacity());
  String inputString = "This is a test String";
  flash.writeStr(strAddr, inputString);
#ifndef __AVR_ATtiny85__
  Serial.print(F("Written string: "));
  Serial.println(inputString);
  Serial.print(F("To address: "));
  Serial.println(strAddr);
#endif
  String outputString = "";
  if (flash.readStr(strAddr, outputString)) {
#ifndef __AVR_ATtiny85__
    Serial.print(F("Read string: "));
    Serial.println(outputString);
    Serial.print(F("From address: "));
    Serial.println(strAddr);
#endif
  }
  while (!flash.eraseSector(strAddr));
}

void loop() {

}

#ifndef __AVR_ATtiny85__
//Reads a string from Serial
bool readSerialStr(String &inputStr) {
  if (!Serial)
    Serial.begin(115200);
  while (Serial.available()) {
    inputStr = Serial.readStringUntil('\n');
    Serial.println(inputStr);
    return true;
  }
  return false;
}
#endif
