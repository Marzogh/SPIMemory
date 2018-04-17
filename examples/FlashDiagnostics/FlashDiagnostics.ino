/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             FlashDiagnostics.ino                                                              |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 3.1.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  04.03.2018                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                 uncomment #define RUNDIAGNOSTIC in SPIFlash.h in the library before compiling                                 |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include<SPIMemory.h>

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#if defined (SIMBLEE)
#define BAUD_RATE 250000
#define RANDPIN 1
#else
#define BAUD_RATE 115200
#if defined(ARCH_STM32)
#define RANDPIN PA0
#else
#define RANDPIN A0
#endif
#endif

#define TRUE 1
#define FALSE 0

//SPIFlash flash;                   //Use this constructor if your Flash chip does not support the SFDP standard - autoidentification will no longer work and you will be restricted to the flash memory chips listed in ReadMe.md
//SFDPFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus
SFDPFlash flash;                    

void setup() {
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_ARCH_SAMD) || (__AVR_ATmega32U4__) || defined(ARCH_STM32)
  while (!Serial) ; // Wait for Serial monitor to open
#endif
  delay(50); //Time to terminal get connected
  Serial.print(F("Initialising Flash memory"));
  for (uint8_t i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  randomSeed(analogRead(RANDPIN));
  //while (!
  flash.begin();/*) {
    delay(1000);
  }*/
  //To use a custom flash memory size (if using memory from manufacturers not officially supported by the library) - declare a size variable according to the list in defines.h
  //flash.begin(MB(1));
  Serial.println();
  Serial.println();

  if (getID()) {
    eraseChipTest();
    eraseSectionTest();
    eraseBlock64KTest();
    eraseBlock32KTest();
    eraseSectorTest();
    Serial.println();

#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP8266)
    delay(10);
    powerDownTest();
    powerUpTest();
    Serial.println();
#endif

    byteTest();
    charTest();
    wordTest();
    shortTest();
    uLongTest();
#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP8266)
    delay(10);
#endif
    longTest();
    floatTest();
    structTest();
    arrayTest();
    stringTest();

#if !defined(ARDUINO_ARCH_SAM) || !defined(ARDUINO_ARCH_ESP8266)
    Serial.println();
    powerDownTest();
    powerUpTest();
#endif
    printLine();
    if (!flash.functionRunTime()) {
      Serial.println(F("Please uncomment RUNDIAGNOSTIC in SPIFlash.h to see the time taken by each function to run."));
    }
  }
}

void loop() {

}

