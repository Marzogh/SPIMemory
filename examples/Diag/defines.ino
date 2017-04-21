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

#define STARTADDR ((sizeof(dataPacket))*12)

uint32_t addr;
float startTime;

struct _dataPacket {
  float wTime;
  float rTime;
  float wNCTime;
  uint8_t Status;
};
_dataPacket dataPacket;
#else
//************************************************************************************************//
//                                                                                                //
//                                   If using an ATTiny85 board                                   //
//                                                                                                //
//************************************************************************************************//
uint32_t addr;
uint8_t _status;
#define STARTADDR ((sizeof(_status))*11)
#endif
//************************************************************************************************//
//                                                                                                //
//                                    Non-board specific code                                     //
//                                                                                                //
//************************************************************************************************//
#define PASS 0x01
#define FAIL 0x00
#define ATTINY85 0b00000010
