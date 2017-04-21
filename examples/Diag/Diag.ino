/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                           tinyFlashDiagnostics.ino                                                            |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.7.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  20.04.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program writes a struct to a random location on your flash memory chip and reads it back.                         |
  | This particular program is designed to test the library with an ATTiny85 chip. Follow the steps below to make sure the test runs as it should |
  |                         1. Compile and run this program on an ATTiny85 board to which an SPIFlash memory has been wired in.                   |
  |                     2. Swap Flash chip over to another supported board (with Serial IO) and compile and run this program again                |
  |                                   3. Check serial output to see if data has been written/read properly.                                       |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include<SPIFlash.h>

//Define a flash memory size (if using non-Winbond memory) according to the list in defines.h
//#define CHIPSIZE MB64

SPIFlash flash;

void setup() {
  //************************************************************************************************//
  //                                                                                                //
  //                                 If using a non ATTiny85 board                                  //
  //                                                                                                //
  //************************************************************************************************//
#if !defined (__AVR_ATtiny85__)
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
  Serial.println();
  Serial.println();

#if defined (CHIPSIZE)
  flash.begin(CHIPSIZE); //use flash.begin(CHIPSIZE) if using non-Winbond flash (Refer to '#define CHIPSIZE' above)
#else
  flash.begin();
#endif

#if defined (ARDUINO_ARCH_ESP32)
  randomSeed(65535537);
#else
  randomSeed(analogRead(RANDPIN));
#endif

  if (!prevWritten()) {
    diagnose();
  }
  else {
    printResults();
#else
  //************************************************************************************************//
  //                                                                                                //
  //                                   If using an ATTiny85 board                                   //
  //                                                                                                //
  //************************************************************************************************//
#if defined (CHIPSIZE)
    flash.begin(CHIPSIZE); //use flash.begin(CHIPSIZE) if using non-Winbond flash (Refer to '#define CHIPSIZE' above)
#else
    flash.begin();
#endif
if (!prevWritten()) {
    diagnose();
  }
#endif

  }

  void loop() {
  }
