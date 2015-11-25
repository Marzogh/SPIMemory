/*
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                Diagnostics.ino                                                                |
|                                                               SPIFlash library                                                                |
|                                                                   v 2.2.0                                                                     |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                    Marzogh                                                                    |
|                                                                  25.11.2015                                                                   |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                                                                                               |
|                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
|                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
|                                             and loading this application onto your Arduino.                                                   |
|                                                                                                                                               |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Serial Print Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void clearprintBuffer()
{
  for (uint8_t i = 0; i < 128; i++) {
    printBuffer[i] = 0;
  }
}

void printLine() {
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
}

void printPass() {
  Serial.println(F("Pass"));
}

void printFail() {
  Serial.println(F("Fail"));
}

void printTab(uint8_t a, uint8_t b) {
  for (uint8_t i = 0; i < a; i++) {
    Serial.print(F("\t"));
  }
  if (b > 0) {
    Serial.print("||");
    for (uint8_t i = 0; i < b; i++) {
      Serial.print(F("\t"));
    }
  }
}
