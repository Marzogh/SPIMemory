/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                Diagnostics.ino                                                                |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.5.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  30.09.2016                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include<SPIFlash.h>

SPIFlash flash;

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
  getID();
  diagnose();
}

void loop() {

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Serial Print Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void clearprintBuffer(char *bufPtr)
{
  for (uint8_t i = 0; i < 128; i++) {
    //printBuffer[i] = 0;
    *bufPtr++ = 0;
  }
}

void printLine() {
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print(F("-"));
  }
  Serial.println();
}

void printPass() {
  Serial.print(F("Pass"));
}

void printFail() {
  Serial.print(F("Fail"));
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

void printTime(uint32_t _wTime, uint32_t _rTime) {
  printTab(2, 1);
  printTimer(_wTime);
  printTab(2, 1);
  printTimer(_rTime);
}

void printTimer(uint32_t _us) {

  if (_us > 1000000) {
    float _s = _us / (float)1000000;
    Serial.print(_s, 4);
    Serial.print(" s");
  }
  else if (_us > 10000) {
    float _ms = _us / (float)1000;
    Serial.print(_ms, 4);
    Serial.print(" ms");
  }
  else {
    Serial.print(_us);
    Serial.print(F(" us"));
  }
}
