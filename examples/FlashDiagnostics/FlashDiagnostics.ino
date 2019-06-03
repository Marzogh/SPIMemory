/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             FlashDiagnostics.ino                                                              |
  |                                                               SPIMemory library                                                                |
  |                                                                   v 3.4.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  01.06.2019                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                 uncomment #define RUNDIAGNOSTIC in SPIMemory.h in the library before compiling                                 |
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

int8_t SPIPins[4] = {-1, -1, -1, 33};

SPIFlash flash;
//SPIFlash flash(SPIPins);
//SPIFlash flash(33);
//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus

void setup() {

  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_ARCH_SAMD) || defined (__AVR_ATmega32U4__) || defined (ARCH_STM32) || defined (NRF5) || defined (ARDUINO_ARCH_ESP32)
  while (!Serial) ; // Wait for Serial monitor to open
#endif

  delay(50); //Time to terminal get connected
  Serial.print(F("Initialising"));
  for (uint8_t i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  randomSeed(analogRead(RANDPIN));
  
  if (flash.error()) {
    Serial.println(flash.error(VERBOSE));
  }
  
  flash.begin();
  //To use a custom flash memory size (if using memory from manufacturers not officially supported by the library) - declare a size variable according to the list in defines.h
  //flash.begin(MB(32));

  if (getID()) {

    printLine();
    printTab(7);
    Serial.print("Testing library code");
    printLine();
    printTab(3);
    Serial.print("Function");
    printTab(2);
    Serial.print("Test result");
    printTab(3);
    Serial.print("     Runtime");
    printLine();

    powerDownTest();
    Serial.println();
    powerUpTest();
    Serial.println();

    Serial.println();

    eraseChipTest();
    Serial.println();
    eraseSectionTest();
    Serial.println();
    eraseBlock64KTest();
    Serial.println();
    eraseBlock32KTest();
    Serial.println();
    eraseSectorTest();
    //Serial.println();


    /* #if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP8266)
        delay(10);
        powerDownTest();
        powerUpTest();
        Serial.println();
      #endif */
    printLine();
    printTab(3);
    Serial.print("Data type");
    printTab(2);
    Serial.print("I/O Result");
    printTab(1);
    Serial.print("      Write time");
    printTab(1);
    Serial.print("      Read time");
    printLine();

    byteTest();
    Serial.println();
    charTest();
    Serial.println();
    wordTest();
    Serial.println();
    shortTest();
    Serial.println();
    uLongTest();
    Serial.println();
    /*#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP8266)
        delay(10);
      #endif */
    longTest();
    Serial.println();
    floatTest();
    Serial.println();
    structTest();
    Serial.println();
    arrayTest();
    Serial.println();
    stringTest();

    /* #if !defined(ARDUINO_ARCH_SAM) || !defined(ARDUINO_ARCH_ESP8266)
        Serial.println();
        powerDownTest();
        powerUpTest();
      #endif */
    printLine();
    if (!flash.functionRunTime()) {
      Serial.println(F("To see function runtimes ncomment RUNDIAGNOSTIC in SPIMemory.h."));
    }
  }
}

void loop() {

}
