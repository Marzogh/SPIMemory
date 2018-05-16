/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                               getAddressEx.ino                                                                |
  |                                                               SPIMemory library                                                                |
  |                                                                   v 3.0.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  17.11.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  This program shows the method to use the getAddress() function to automate                                   |
  |                                the process of address allocation when using a flash memory module. Please note                                |
  |                                         the special function used to get the size of the String object.                                       |
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

#define arrayLen(x) sizeof(x)/sizeof(x[0])
uint32_t strAddr[3], floatAddr[2], byteAddr[4];
String testStr[] = {
  "Test String 0",
  "Test String 1",
  "Test String 2"
};
float testFloat[] = {
  3.1415, 6.283
};
byte testByte[] = {
  3, 245, 84, 100
};

//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus
SPIFlash flash;

void getAddresses();
void writeData();

void setup() {
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_ARCH_SAMD) || (__AVR_ATmega32U4__) || defined(ARCH_STM32)
  while (!Serial) ; // Wait for Serial monitor to open
#endif
  delay(50); //Time to terminal get connected
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  flash.begin();
  Serial.println();
  Serial.println();

  getAddresses();
  dataIO();
  //flash.eraseChip();      // Uncomment this if you would like to erase chip
}

void loop() {

}

// Function to get adresses for various variables
void getAddresses() {
  for (uint8_t i = 0; i < arrayLen(byteAddr); i++) {
    byteAddr[i] = flash.getAddress(sizeof(byte));
    Serial.print("Byte Address ");
    Serial.print(i);
    Serial.print(" : 0x");
    Serial.println(byteAddr[i], HEX);
  }

  for (uint8_t i = 0; i < arrayLen(floatAddr); i++) {
    floatAddr[i] = flash.getAddress(sizeof(float));
    Serial.print("Float Address ");
    Serial.print(i);
    Serial.print(" : 0x");
    Serial.println(floatAddr[i], HEX);
  }

  for (uint8_t i = 0; i < arrayLen(strAddr); i++) {
    strAddr[i] = flash.getAddress(flash.sizeofStr(testStr[i]));
    Serial.print("String Address ");
    Serial.print(i);
    Serial.print(" : 0x");
    Serial.println(strAddr[i], HEX);
  }
}

// Function to write data
void dataIO() {
  uint8_t _byte;
  float _float;
  String _string;
  for (uint8_t i = 0; i < arrayLen(byteAddr); i++) {
    if (flash.writeByte(byteAddr[i], testByte[i])) {
      Serial.print(testByte[i]);
      Serial.print(" written to 0x");
      Serial.println(byteAddr[i], HEX);
    }
    _byte = flash.readByte(byteAddr[i]);
    Serial.print(_byte);
    Serial.print(" read from 0x");
    Serial.println(byteAddr[i], HEX);
  }

  for (uint8_t i = 0; i < arrayLen(floatAddr); i++) {
    if (flash.writeFloat(floatAddr[i], testFloat[i])) {
      Serial.print(testFloat[i]);
      Serial.print(" written to 0x");
      Serial.println(floatAddr[i], HEX);
    }
    _float = flash.readFloat(floatAddr[i]);
    Serial.print(_float);
    Serial.print(" read from 0x");
    Serial.println(floatAddr[i], HEX);
  }

  for (uint8_t i = 0; i < arrayLen(strAddr); i++) {
    if (flash.writeStr(strAddr[i], testStr[i])) {
      Serial.print("'");
      Serial.print(testStr[i]);
      Serial.print("' written to 0x");
      Serial.println(strAddr[i], HEX);
    }
    flash.readStr(strAddr[i], _string);
    Serial.print("'");
    Serial.print(_string);
    Serial.print("' read from 0x");
    Serial.println(strAddr[i], HEX);
    _string = "";
  }
}
