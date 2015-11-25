/*
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                             readWriteString.ino                                                               |
|                                                               SPIFlash library                                                                |
|                                                                   v 2.2.0                                                                     |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                    Marzogh                                                                    |
|                                                                  25.11.2015                                                                   |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                                                                                               |
|                        This program shows the method of reading a string from the console and saving it to flash memory                       |
|                                                                                                                                               |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
#include<SPIFlash.h>
#include<SPI.h>

#define cs 10

int strPage, strSize;
byte strOffset;

SPIFlash flash(cs);

void setup() {
  Serial.begin(115200);

  flash.begin();

  Serial.println(F("Please type the string into the console"));
  randomSeed(analogRead(A0));
  strPage = random(0, 4095);
  strOffset = random(0, 255);
  String inputString;
  while (!readSerialStr(inputString));
  flash.writeStr(strPage, strOffset, inputString);
  Serial.print(F("Written string: "));
  Serial.print(inputString);
  Serial.print(F(" to page "));
  Serial.print(strPage);
  Serial.print(F(", at offset "));
  Serial.println(strOffset);
  String outputString = "";
  flash.readStr(strPage, strOffset, outputString);
  Serial.print(F("Read string: "));
  Serial.print(outputString);
  Serial.print(F(" from page "));
  Serial.print(strPage);
  Serial.print(F(", at offset "));
  Serial.println(strOffset);
  flash.eraseSector(strPage, 0);
}

void loop() {

}

//Reads a string from Serial
bool readSerialStr(String &inputStr) {
  if (!Serial)
    Serial.begin(115200);
  while (Serial.available()) {
    inputStr = Serial.readStringUntil('\n');
    return true;
  }
  return false;
}

