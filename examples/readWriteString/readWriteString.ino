/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             readWriteString.ino                                                               |
  |                                                               SPIMemory library                                                                |
  |                                                                   v 3.5.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  13.07.2019                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program shows the method of reading a string from the console and saving it to flash memory                       |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
#include<SPIMemory.h>

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

//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus
SPIFlash flash;

bool readSerialStr(String &inputStr);

void setup() {
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_SAMD_ZERO) || (__AVR_ATmega32U4__)
  while (!Serial) ; // Wait for Serial monitor to open
#endif

  flash.begin();

  randomSeed(analogRead(RANDPIN));
  strAddr = random(0, flash.getCapacity());
  //strAddr = 4500;
  String inputString = "This is a test String";
  //String inputString = R"({"Loc":"BACK","SL:NO":1.3,"ImgID":3,"C ON":"OPEN CONDITION","Qtion":"Hey, this is Device A from location Delhi ID 24A612334"})";
  //char inArray[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
  //if (flash.writeCharArray(strAddr, inArray, 6)) {
  if (flash.writeStr(strAddr, inputString)) {
    Serial.print(F("Written data: "));
  }
  else {
    Serial.print(F("Unable to write data: "));
  }
  Serial.print(inputString);
  /*for (uint8_t i = 0; i < 6; i++) {
    Serial.print(inArray[i]);
    Serial.print(", ");
  }*/
  Serial.print(F(" to address: 0x"));
  Serial.println(strAddr, HEX);

  String outputString = "";
  char outArray[6];
  //if (flash.readCharArray(strAddr, outArray, 6)) {
  if (flash.readStr(strAddr, outputString)){
    Serial.print(F("Read data: "));
    Serial.print(outputString);
    /*for (uint8_t i = 0; i < 6; i++) {
      Serial.print(outArray[i]);
      Serial.print(", ");
    }*/
    Serial.print(F(" from address: 0x"));
    Serial.println(strAddr, HEX);
  }
  else {
    Serial.println("Unable to read String");
  }
  while (!flash.eraseSector(strAddr));
}

void loop() {

}

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
