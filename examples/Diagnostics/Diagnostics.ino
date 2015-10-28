//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                                Diagnostics.ino                                                                //
//                                                               SPIFlash library                                                                //
//                                                                   v 2.2.0                                                                     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                                                                    Marzogh                                                                    //
//                                                                   14.10.15                                                                    //
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
uint8_t pageInputBuffer[256], pageOutputBuffer[256];
char printBuffer[128];

struct Test {
  word s1;
  float s2;
  long s3;
  bool s4;
  byte s5;
};
Test inputStruct;
Test outputStruct;

uint16_t bytePage, charPage, wordPage, shortPage, ULongPage, longPage, floatPage, stringPage, structPage, page;
uint8_t byteOffset, charOffset, wordOffset, shortOffset, ULongOffset, longOffset, floatOffset, stringOffset, structOffset;

uint8_t _byte = 35;
int8_t _char = -110;
uint16_t _word = 4520;
int16_t _short = -1250;
uint32_t _uLong = 876532;
int32_t _long = -10959;
float _float = 3.1415;
String _string = "123 Test !@#";

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

  randomSeed(analogRead(A0));

  for (int i = 0; i < 256; ++i) {
    pageInputBuffer[i] = i;
  }

  getAddresses();
  ID();
  writeData();
  checkData();
  checkFunctions();
}

void loop() {

}

void getAddresses() {
  page = random(0, 4095);
  bytePage = random(0, 4095);
  byteOffset = random(0, 255);
  charPage = random(0, 4095);
  charOffset = random(0, 255);
  wordPage = random(0, 4095);
  wordOffset = random(0, 255);
  shortPage = random(0, 4095);
  shortOffset = random(0, 255);
  ULongPage = random(0, 4095);
  ULongOffset = random(0, 255);
  longPage = random(0, 4095);
  longOffset = random(0, 255);
  floatPage = random(0, 4095);
  floatOffset = random(0, 255);
  stringPage = random(0, 4095);
  stringOffset = random(0, 255);
  structPage = random(0, 4095);
  structOffset = random(0, 255);

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

  inputStruct.s1 = 31325;
  inputStruct.s2 = 4.84;
  inputStruct.s3 = 880932;
  inputStruct.s4 = true;
  inputStruct.s5 = 5;

  flash.writeByte(bytePage, byteOffset, _byte);
  flash.writeChar(charPage, charOffset, _char);
  flash.writeWord(wordPage, wordOffset, _word);
  flash.writeShort(shortPage, shortOffset, _short);
  flash.writeULong(ULongPage, ULongOffset, _uLong);
  flash.writeLong(longPage, longOffset, _long);
  flash.writeFloat(floatPage, floatOffset, _float);
  flash.writeStr(stringPage, stringOffset, _string);
  flash.writeAnything(structPage, structOffset, inputStruct);
  flash.writePage(page, pageInputBuffer);
  clearprintBuffer();
  sprintf(printBuffer, "Values from 0 to 255 have been written to the page %d", page);
  Serial.println(printBuffer);
}

void checkData() {

  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("                                                          Data Check                                                              "));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));

  Serial.println(F("\tData Written\t||\t\tData Read\t\t||\t\tResult"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));

  Serial.print(F("\t"));
  Serial.print(_byte);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readByte(bytePage, byteOffset));
  Serial.print(F("\t\t\t||\t\t"));
  if (_byte == flash.readByte(bytePage, byteOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_char);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readChar(charPage, charOffset));
  Serial.print(F("\t\t\t||\t\t"));
  if (_char == flash.readChar(charPage, charOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_word);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readWord(wordPage, wordOffset));
  Serial.print(F("\t\t\t||\t\t"));
  if (_word == flash.readWord(wordPage, wordOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_short);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readShort(shortPage, shortOffset));
  Serial.print(F("\t\t\t||\t\t"));
  if (_short == flash.readShort(shortPage, shortOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_uLong);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readULong(ULongPage, ULongOffset));
  Serial.print(F("\t\t\t||\t\t"));
  if (_uLong == flash.readULong(ULongPage, ULongOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_long);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readLong(longPage, longOffset));
  Serial.print(F("\t\t\t||\t\t"));
  if (_long == flash.readLong(longPage, longOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_float, 4);
  Serial.print(F("\t\t||\t\t"));
  Serial.print(flash.readFloat(floatPage, floatOffset), 4);
  Serial.print(F("\t\t\t||\t\t"));
  if (_float == flash.readFloat(floatPage, floatOffset))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(_string);
  Serial.print(F("\t||\t\t"));
  String outString;
  flash.readStr(stringPage, stringOffset, outString);
  Serial.print(outString);
  Serial.print(F("\t\t||\t\t"));
  if (_string == outString)
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(F("inputStruct"));
  Serial.print(F("\t||\t\t"));
  flash.readAnything(structPage, structOffset, outputStruct);
  Serial.print(F("outputStruct"));
  Serial.print(F("\t\t||\t\t"));
  if (inputStruct.s1 == outputStruct.s1 && inputStruct.s2 == outputStruct.s2 && inputStruct.s3 == outputStruct.s3 && inputStruct.s4 == outputStruct.s4 && inputStruct.s5 == outputStruct.s5)
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t"));
  Serial.print(F("0 - 255"));
  Serial.print(F("\t\t||\t\t"));
  if (flash.readPage(page, pageOutputBuffer))
    Serial.print(F("0 - 255"));
  else
    Serial.print(F("Unknown"));
  Serial.print(F("\t\t\t||\t\t"));
  if (checkPage())
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));
}

void checkFunctions() {
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("                                                        Check Functions                                                           "));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));

  Serial.println(F("\t\t\tFunction\t\t||\t\tResult"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));


  testPowerFunc();
  if (flash.eraseChip())
    Serial.println (F("Chip erased successfully"));
  else
    Serial.println (F("Chip erase unsuccessful"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void clearprintBuffer()
{
  for (uint8_t i = 0; i < 128; i++) {
    printBuffer[i] = 0;
  }
}

//Prints hex/dec formatted data from page reads - for debugging
void _printPageBytes(uint8_t *data_buffer, uint8_t outputType) {
  char buffer[10];
  for (int a = 0; a < 16; ++a) {
    for (int b = 0; b < 16; ++b) {
      if (outputType == 1) {
        sprintf(buffer, "%02x", data_buffer[a * 16 + b]);
        Serial.print(buffer);
      }
      else if (outputType == 2) {
        uint8_t x = data_buffer[a * 16 + b];
        if (x < 10) Serial.print(F("0"));
        if (x < 100) Serial.print(F("0"));
        Serial.print(x);
        Serial.print(',');
      }
    }
    Serial.println();
  }
}

//Reads a page of data and prints it to Serial stream. Make sure the sizeOf(uint8_t data_buffer[]) == 256.
void printPage(uint16_t page_number, uint8_t outputType) {
  if (!Serial)
    Serial.begin(115200);

  char buffer[24];
  sprintf(buffer, "Reading page (%04x)", page_number);
  Serial.println(buffer);

  uint8_t data_buffer[256];
  flash.readPage(page_number, data_buffer);
  _printPageBytes(data_buffer, outputType);
}

bool checkPage() {
  for (int i = 0; i < 256; i++) {
    if (pageInputBuffer[i] != pageOutputBuffer[i])
      return false;
  }
  return true;
}

void testPowerFunc() {
  uint32_t capacity = flash.getCapacity();
  String testString = "This is a inputStruct String";
  if (!Serial)
    Serial.begin(115200);
  uint32_t stringAddress1 = random(0, capacity);
  uint32_t stringAddress2 = random(0, capacity);
  uint32_t stringAddress3 = random(0, capacity);

  Serial.print(F("\t\t\t"));
  Serial.print(F("PowerDown"));
  Serial.print(F("\t\t||\t\t"));
  if (flash.writeStr(stringAddress1, testString) && flash.powerDown() && !flash.writeStr(stringAddress2, testString))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  Serial.print(F("\t\t\t"));
  Serial.print(F("PowerUp"));
  Serial.print(F("\t\t\t||\t\t"));
  if (flash.powerUp() && flash.writeStr(stringAddress3, testString))
    Serial.println(F("Pass"));
  else
    Serial.println(F("Fail"));

  if (flash.eraseSector(stringAddress1) && flash.eraseSector(stringAddress2) && flash.eraseSector(stringAddress3))
    Serial.println(F("Sectors erased successfully"));
  else
    Serial.println(F("Sector erase unsuccessful"));
}

