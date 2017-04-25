/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             FlashDiagnostics.ino                                                              |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.6.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  16.04.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

//************************************************************************************************//
//                                                                                                //
//                                    Non-board specific code                                     //
//                                                                                                //
//************************************************************************************************//

#include<SPIFlash.h>

//Define a flash memory size (if using non-Winbond memory) according to the list in defines.h
//#define CHIPSIZE MB64

#define PASS 0x01
#define FAIL 0x00
#define ATTINY85 0x02

#define INT          0x01
#define FLOAT        0x02
#define STRING       0x04
#define STRUCT       0x08
#define ARRAY        0x10
#define ERASE        0x20
#define POWER        0x40

struct _dataPacket {
uint8_t test;
uint16_t Status;
};
_dataPacket dataPacket;

//************************************************************************************************//
//                                                                                                //
//                                 If using a non ATTiny85 board                                  //
//                                                                                                //
//************************************************************************************************//
#if !defined (__AVR_ATtiny85__)
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

void setup() {
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_ARCH_SAMD) || (__AVR_ATmega32U4__)
  while (!Serial) ; // Wait for Serial monitor to open
#endif
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
#if defined (CHIPSIZE)
  flash.begin(CHIPSIZE); //use flash.begin(CHIPSIZE) if using non-Winbond flash (Refer to '#define CHIPSIZE' above)
#else
  flash.begin();
#endif
  Serial.println();
  Serial.println();

#if defined (ARDUINO_ARCH_ESP32)
  randomSeed(65535537);
#else
  randomSeed(analogRead(RANDPIN));
#endif
  if (!prevWritten()) {
    getID();
    diagnose();
  }
  else {
    tinyResult();
  }
}

void loop() {

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Serial Print Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void clearprintBuffer(char *bufPtr)
{
  for (uint8_t i = 0; i < 128; i++) {
    //printBuffer[i] = 0;
    *bufPtr++ = 0;
  }
}

void printLine() {
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print(F("-"));
  }
  Serial.println();
}

void printPass() {
  Serial.print(F("Pass"));
}

void printFail() {
  Serial.print(F("Fail"));
}

void printTab(uint8_t a, uint8_t b) {
  for (uint8_t i = 0; i < a; i++) {
    Serial.print(F("\t"));
  }
  if (b > 0) {
    Serial.print("||");
    for (uint8_t i = 0; i < b; i++) {
      Serial.print(F("\t"));
    }
  }
}

void printTime(uint32_t _wTime, uint32_t _rTime) {
  printTab(2, 1);
  printTimer(_wTime);
  printTab(2, 1);
  printTimer(_rTime);
}

void printTimer(uint32_t _us) {

  if (_us > 1000000) {
    float _s = _us / (float)1000000;
    Serial.print(_s, 4);
    Serial.print(" s");
  }
  else if (_us > 10000) {
    float _ms = _us / (float)1000;
    Serial.print(_ms, 4);
    Serial.print(" ms");
  }
  else {
    Serial.print(_us);
    Serial.print(F(" us"));
  }
}
#endif


//************************************************************************************************//
//                                                                                                //
//                                   If using an ATTiny85 board                                   //
//                                                                                                //
//************************************************************************************************//

#if defined (__AVR_ATtiny85__)

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
  //                                                                                            //
  //                       ___ _____ _____ _             _____  _____                           //
  //                      / _ \_   _|_   _(_)           |  _  ||  ___|                          //
  //                     / /_\ \| |   | |  _ _ __  _   _ \ V / |___ \                           //
  //                     |  _  || |   | | | | '_ \| | | |/ _ \     \ \                          //
  //                     | | | || |   | | | | | | | |_| | |_| |/\__/ /                          //
  //                     \_| |_/\_/   \_/ |_|_| |_|\__, \_____/\____/                           //
  //                                                __/ |                                       //
  //                                               |___/                                        //
  //                                                                                            //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //                       Uncomment any of the tests below as required.                        //
  //        Remember, the ATTiny has a limited amount of SRAM, so only a limited number         //
  //                             of tests can be run concurrently.                              //
  //                                                                                            //
  //|------------------------------------------//----------------------------------------------|//
  //|                     List of tests        //                 _status bits                 |//
  //|------------------------------------------//----------------------------------------------|//
  #define INTTEST                            //                     iW, iR                   |//
  #define FLOATTEST                          //                     fW, fR                   |//
  //#define STRINGTEST                         //                    sgW, sgR                  |//
  #define STRUCTTEST                         //                    scW, scR                  |//
  //#define ARRAYTEST                          //                     aW, aR                   |//
  #define ERASETEST                          //                     eB, eC                   |//
  #define POWERTEST                          //                    pON, pOFF                 |//
  //|------------------------------------------//----------------------------------------------|//
 
uint32_t addr;

#define STARTADDR    ((sizeof(_status))*11)

#define iW    0x0001
#define iR    0x0002
#define fW    0x0004
#define fR    0x0008
#define sgW   0x0010
#define sgR   0x0020
#define scW   0x0040
#define scR   0x0080
#define aW    0x0100
#define aR    0x0200
#define eB    0x0400
#define eC    0x0800
#define pON   0x1000
#define pOFF  0x2000

SPIFlash flash;

void setup() {
#if defined (CHIPSIZE)
  flash.begin(CHIPSIZE); //use flash.begin(CHIPSIZE) if using non-Winbond flash (Refer to '#define CHIPSIZE' above)
#else
  flash.begin();
#endif
  randomSeed(analogRead(1));
  if (!prevWritten()) {
    diagnose();
  }
}

void loop() {
}
#endif
