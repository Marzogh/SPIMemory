/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                               Struct_writer.ino                                                               |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.7.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  19.04.2017                                                                   |
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

SPIFlash flash;

struct Configuration {
  float lux;
  float vOut;                   // Voltage ouput from potential divider to Analog input
  float RLDR;                   // Resistance calculation of potential divider with LDR
  bool light;
  uint8_t adc;
};
Configuration configuration;

void setup() {
  flash.begin();
  uint32_t _addr = 34235;

#if defined (__AVR_ATtiny85__)
  configuration.lux = 98.43;
  configuration.vOut = 4.84;
  configuration.RLDR = 889.32;
  configuration.light = true;
  configuration.adc = 5;
  flash.writeAnything(_addr, configuration);
#else
  Serial.println("Data Written to flash was: ");
  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);
  configuration.lux = 0;
  configuration.vOut = 0;
  configuration.RLDR = 0;
  configuration.light = 0;
  configuration.adc = 0;
  flash.readAnything(_addr, configuration);
  flash.eraseSector(_addr, 0);
  Serial.println("After reading");
  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);
#endif

}

void loop() {
}
