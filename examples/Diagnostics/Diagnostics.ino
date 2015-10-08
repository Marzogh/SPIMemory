//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                                Diagnostics.ino                                                                //
//                                                               SPIFlash library                                                                //
//                                                                   v 1.3.2                                                                     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                                    Marzogh                                                                    //
//                                                                   17.09.15                                                                    //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                                                                                                               //
//                                  For a full diagnostics rundown - with error codes and details of the errors                                  //
//                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                //
//                                             and loading this application onto your Arduino.                                                   //
//                                                                                                                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include<SPIFlash.h>
#include<SPI.h>
const int cs = 10;
uint8_t pageBuffer[256];
char printBuffer[128];

struct Test {
  word s1;
  float s2;
  long s3;
  bool s4;
  uint8_t s5;
};
Test test;

uint8_t _byte = 35;
int8_t _char = -110;
uint16_t _word = 4520;
int16_t _short = -1250;
uint32_t _uLong = 876532;
int32_t _long = -10959;
float _float = 3.1415;

SPIFlash flash(cs);

void setup() {
  Serial.begin(115200);
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  flash.begin();
  Serial.println();
  Serial.println();

  ID();
  writeData();
}

void loop() {

}

void ID() {
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("                                                           Get ID                                                                 "));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  uint8_t b1, b2;
  uint16_t b3;
  uint32_t JEDEC = flash.getJEDECID();
  uint16_t ManID = flash.getManID();
  b1 = (ManID >> 8);
  b2 = (ManID >> 0);
  b3 = (JEDEC >> 0);
  clearprintBuffer();
  sprintf(printBuffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh", b1, b2, b3);
  Serial.println(printBuffer);
  clearprintBuffer();
  sprintf(printBuffer, "JEDEC ID: %04lxh", JEDEC);
  Serial.println(printBuffer);
}

void writeData() {
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("                                                          Write Data                                                              "));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));

  test.s1 = 31325;
  test.s2 = 4.84;
  test.s3 = 880932;
  test.s4 = true;
  test.s5 = 5;

  flash.writeByte(1920, 4, _byte);
  flash.writeChar(1920, 5, _char);
  flash.writeWord(1920, 6, _word);
  flash.writeShort(1920, 8, _short);
  flash.writeULong(1920, 10, _uLong);
  flash.writeLong(1920, 14, _long);
  flash.writeFloat(1920, 18, _float);

  Serial.println("Data Written\t\t||\t\tData Read\t\t||\t\tMatch");
  Serial.println("--------------------------------------------------------------------------------------");

  Serial.print("\t");
  Serial.print(_byte);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readByte(1920, 4));
  Serial.print("\t\t\t||\t\t");
  if (_byte == flash.readByte(1920, 4))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("\t");
  Serial.print(_char);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readChar(1920, 5));
  Serial.print("\t\t\t||\t\t");
  if (_char == flash.readChar(1920, 5))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("\t");
  Serial.print(_word);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readWord(1920, 6));
  Serial.print("\t\t\t||\t\t");
  if (_word == flash.readWord(1920, 6))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("\t");
  Serial.print(_short);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readShort(1920, 8));
  Serial.print("\t\t\t||\t\t");
  if (_short == flash.readShort(1920, 8))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("\t");
  Serial.print(_uLong);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readULong(1920, 10));
  Serial.print("\t\t\t||\t\t");
  if (_uLong == flash.readULong(1920, 10))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("\t");
  Serial.print(_long);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readLong(1920, 14));
  Serial.print("\t\t\t||\t\t");
  if (_long == flash.readLong(1920, 14))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("\t");
  Serial.print(_float, 4);
  Serial.print("\t\t||\t\t");
  Serial.print(flash.readFloat(1920, 18), 4);
  Serial.print("\t\t\t||\t\t");
  if (_float == flash.readFloat(1920, 18))
    Serial.println("True");
  else
    Serial.println("False");

  Serial.print("Saving struct at: ");
  Serial.println(flash.writeAnything(2, 4, test));
  Serial.println("Data written");
  Serial.println(test.s1);
  Serial.println(test.s2);
  Serial.println(test.s3);
  Serial.println(test.s4);
  Serial.println(test.s5);

  Serial.println("Saved!");
  test.s1 = 0;
  test.s2 = 0;
  test.s3 = 0;
  test.s4 = 0;
  test.s5 = 0;
  Serial.println();
  Serial.println("Local values set to 0");
  Serial.println();
  Serial.print("Reading struct back from: ");
  Serial.println(flash.readAnything(2, 4, test));
  flash.eraseSector(2);

  Serial.println("After reading");
  Serial.println(test.s1);
  Serial.println(test.s2);
  Serial.println(test.s3);
  Serial.println(test.s4);
  Serial.println(test.s5);

  for (int i = 0; i < 256; ++i) {
    pageBuffer[i] = i;
  }
  uint16_t page = 836;
  uint8_t hex = 1, dec = 2;
  flash.writePage(page, pageBuffer);
  clearprintBuffer();
  sprintf(printBuffer, "Values from 0 to 255 have been written to the page %d", page);
  Serial.println(printBuffer);
  clearprintBuffer();
  sprintf(printBuffer, "These values have been read back from page %d", page);
  Serial.println(printBuffer);
  flash.printPage(page, dec);
  flash.eraseChip();
}

void clearprintBuffer()
{
  for (uint8_t i = 0; i < 128; i++) {
    printBuffer[i] = 0;
  }
}
