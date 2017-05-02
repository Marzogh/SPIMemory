/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    vars.h                                                                     |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.7.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  26.04.2017                                                                   |
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

struct _dataPacket {
  uint8_t writeStatus;
  uint8_t test;
  uint16_t Status;
};
_dataPacket dataPacket;
uint32_t addr;

//************************************************************************************************//
//                                                                                                //
//                                   If using an ATTiny85 board                                   //
//                                                                                                //
//************************************************************************************************//

#if defined (__AVR_ATtiny85__)

#define STARTADDR    ((sizeof(dataPacket))*2)

#endif

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

#define DATA_FUNCTION 0x03
#define OTHER_FUNCTION 0x04

bool data_header, power_header = false;

#endif

