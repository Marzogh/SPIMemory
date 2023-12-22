/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                FlashAsyc.ino                                                                  |
  |                                                               SPIMemory library                                                               |
  |                                                                   v 3.5.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             LithosphereRocketry                                                               |
  |                                                                  21.12.2023                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program shows the method of reading a string from the console and saving it to flash memory                       |
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
#else
#define BAUD_RATE 115200
#endif

uint8_t someData[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus
SPIFlash flash;

void setup() {
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_SAMD_ZERO) || (__AVR_ATmega32U4__)
  while (!Serial) ; // Wait for Serial monitor to open
#endif
  
  flash.begin();
  
  // Erase the block we will be writing to
  flash.eraseBlock64K(0);

  // Start erasing the next block
  flash.eraseBlock64K(0x10000);
  for(int i = 0; i < 10; i++) {
    // Periodically while the erase is happening, pause it and write some data
    // to the first block
    flash.suspendProg();
    // We can only read or write while suspended, no erasing
    flash.writeByteArray(i*100, someData, 10);
    flash.resumeProg();
    delay(10);
  }

  // Clean up the block we wrote to
  flash.eraseBlock64K(0);
}

void loop() {

}