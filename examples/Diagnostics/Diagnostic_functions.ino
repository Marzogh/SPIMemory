/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                            Diagnostic_functions.ino                                                           |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.3.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  04.06.2016                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

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
  printLine();
  Serial.println(F("                                                                               Get ID                                                                 "));
  printLine();
  uint8_t b1, b2;
  uint16_t b3;
  uint32_t JEDEC = flash.getJEDECID();
  uint16_t ManID = flash.getManID();
  uint32_t maxPage = flash.getMaxPage();
  uint16_t _name = flash.getChipName();

  //---------------------------------------------------------------------------------------------//
  //--------------------------Prints the name of the Flash chip in use---------------------------//
  //---------------------------------------------------------------------------------------------//
  Serial.print(F("                                                                          Winbond "));
  if (_name < 80) {
    if (_name == 05) {
      clearprintBuffer();
      sprintf(printBuffer, "W25X%02dCL", _name);
      Serial.println(printBuffer);
      clearprintBuffer();
    }
    else if (_name % 10 == 0) {
      clearprintBuffer();
      sprintf(printBuffer, "W25X%02dBV", _name);
      Serial.println(printBuffer);
      clearprintBuffer();
    }
    else {
      clearprintBuffer();
      sprintf(printBuffer, "W25Q%02dBV", _name);
      Serial.println(printBuffer);
      clearprintBuffer();
    }
  }
  else {
    clearprintBuffer();
    sprintf(printBuffer, "W25Q%02dBV", _name);
    Serial.println(printBuffer);
    clearprintBuffer();
  }
  printLine();
  //---------------------------------------------------------------------------------------------//

  b1 = (ManID >> 8);
  b2 = (ManID >> 0);
  b3 = (JEDEC >> 0);
  clearprintBuffer();
  sprintf(printBuffer, "\t\t\tJEDEC ID: %04lxh", JEDEC);
  Serial.println(printBuffer);
  clearprintBuffer();
  sprintf(printBuffer, "\t\t\tManufacturer ID: %02xh\n\t\t\tMemory Type: %02xh\n\t\t\tCapacity: %02xh\n\t\t\tMaximum pages: %lu", b1, b2, b3, maxPage);
  Serial.println(printBuffer);
}

void writeData() {
  printLine();
  Serial.println(F("                                                                             Write Data                                                              "));
  printLine();

  inputStruct.s1 = 31325;
  inputStruct.s2 = 4.84;
  inputStruct.s3 = 880932;
  inputStruct.s4 = true;
  inputStruct.s5 = 5;

  startTimer();
  flash.writeByte(bytePage, byteOffset, _byte);
  writeTimer._byte = getTimer();

  startTimer();
  flash.writeChar(charPage, charOffset, _char);
  writeTimer._char = getTimer();

  startTimer();
  flash.writeWord(wordPage, wordOffset, _word);
  writeTimer._word = getTimer();

  startTimer();
  flash.writeShort(shortPage, shortOffset, _short);
  writeTimer._short = getTimer();

  startTimer();
  flash.writeULong(ULongPage, ULongOffset, _uLong);
  writeTimer._ulong = getTimer();

  startTimer();
  flash.writeLong(longPage, longOffset, _long);
  writeTimer._long = getTimer();

  startTimer();
  flash.writeFloat(floatPage, floatOffset, _float);
  writeTimer._float = getTimer();

  startTimer();
  flash.writeStr(stringPage, stringOffset, _string);
  writeTimer._string = getTimer();

  startTimer();
  flash.writeAnything(structPage, structOffset, inputStruct);
  writeTimer._struct = getTimer();

  startTimer();
  flash.writePage(page, pageInputBuffer);
  writeTimer._page = getTimer();
  Serial.println(F("\t\t\t\tData written without errors"));
}

bool checkPage() {
  for (int i = 0; i < 256; i++) {
    if (pageInputBuffer[i] != pageOutputBuffer[i])
      return false;
  }
  return true;
}


void checkData() {

  printLine();
  Serial.println(F("                                                                              Data Check                                                              "));
  printLine();

  Serial.println(F("\tData Written\t||\t\tData Read\t\t||\t\tResult\t\t\t||\t\tWrite Time\t\t||\t\tRead Time"));
  printLine();

  //Serial.print(F("\t"));
  printTab(1, 0);
  Serial.print(_byte);
  //Serial.print(F("\t\t||\t\t"));
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readByte(bytePage, byteOffset));
  readTimer._byte = getTimer();
  //Serial.print(F("\t\t\t||\t\t"));;
  printTab(3, 2);
  if (_byte == flash.readByte(bytePage, byteOffset))
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._byte);
  printTab(3, 2);
  printTimer(readTimer._byte);
  Serial.println();

  printTab(1, 0);
  Serial.print(_char);
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readChar(charPage, charOffset));
  readTimer._char = getTimer();
  printTab(3, 2);
  if (_char == flash.readChar(charPage, charOffset))
    printPass();
  else
    printFail(); printTab(3, 2);
  printTimer(writeTimer._char);
  printTab(3, 2);
  printTimer(readTimer._char);
  Serial.println();

  printTab(1, 0);
  Serial.print(_word);
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readWord(wordPage, wordOffset));
  readTimer._word = getTimer();
  printTab(3, 2);
  if (_word == flash.readWord(wordPage, wordOffset))
    printPass();
  else
    printFail(); printTab(3, 2);
  printTimer(writeTimer._word);
  printTab(3, 2);
  printTimer(readTimer._word);
  Serial.println();

  printTab(1, 0);
  Serial.print(_short);
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readShort(shortPage, shortOffset));
  readTimer._short = getTimer();
  printTab(3, 2);
  if (_short == flash.readShort(shortPage, shortOffset))
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._short);
  printTab(3, 2);
  printTimer(readTimer._short);
  Serial.println();

  printTab(1, 0);
  Serial.print(_uLong);
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readULong(ULongPage, ULongOffset));
  readTimer._ulong = getTimer();
  printTab(3, 2);
  if (_uLong == flash.readULong(ULongPage, ULongOffset))
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._ulong);
  printTab(3, 2);
  printTimer(readTimer._ulong);
  Serial.println();

  printTab(1, 0);
  Serial.print(_long);
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readLong(longPage, longOffset));
  readTimer._long = getTimer();
  printTab(3, 2);
  if (_long == flash.readLong(longPage, longOffset))
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._long);
  printTab(3, 2);
  printTimer(readTimer._long);
  Serial.println();

  printTab(1, 0);
  Serial.print(_float, 4);
  printTab(2, 2);
  startTimer();
  Serial.print(flash.readFloat(floatPage, floatOffset), 4);
  readTimer._float = getTimer();
  printTab(3, 2);
  if (_float == flash.readFloat(floatPage, floatOffset))
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._float);
  printTab(3, 2);
  printTimer(readTimer._float);
  Serial.println();

  printTab(1, 0);
  Serial.print(_string);
  Serial.print(F("\t||\t\t"));
  String outString;
  startTimer();
  flash.readStr(stringPage, stringOffset, outString);
  readTimer._string = getTimer();
  Serial.print(outString);
  printTab(2, 2);
  if (_string == outString)
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._string);
  printTab(3, 2);
  printTimer(readTimer._string);
  Serial.println();

  printTab(1, 0);
  Serial.print(F("inputStruct"));
  printTab(1, 2);
  startTimer();
  flash.readAnything(structPage, structOffset, outputStruct);
  readTimer._struct = getTimer();
  Serial.print(F("outputStruct"));
  printTab(2, 2);
  if (inputStruct.s1 == outputStruct.s1 && inputStruct.s2 == outputStruct.s2 && inputStruct.s3 == outputStruct.s3 && inputStruct.s4 == outputStruct.s4 && inputStruct.s5 == outputStruct.s5)
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._struct);
  printTab(3, 2);
  printTimer(readTimer._struct);
  Serial.println();

  printTab(1, 0);
  Serial.print(F("0 - 255"));
  printTab(2, 2);
  startTimer();
  if (flash.readPage(page, pageOutputBuffer)) {
    readTimer._page = getTimer();
    Serial.print(F("0 - 255"));
  }
  else
    Serial.print(F("Unknown"));
  printTab(3, 2);
  if (checkPage())
    printPass();
  else
    printFail();
  printTab(3, 2);
  printTimer(writeTimer._page);
  printTab(3, 2);
  printTimer(readTimer._page);
  Serial.println();
}

void checkFunctions() {
  printLine();
  Serial.println(F("                                                                        Check Other Functions                                                        "));
  printLine();
  Serial.println(F("\t\t\t\t\tFunction\t\t||\t\tResult\t\t\t||\t\tTime"));
  printLine();

  uint32_t capacity = flash.getCapacity();
  if (!Serial)
    Serial.begin(115200);
  uint32_t stringAddress1 = random(0, capacity);
  uint32_t stringAddress2 = random(0, capacity);
  uint32_t stringAddress3 = random(0, capacity);

  printTab(5, 0);
  Serial.print(F("powerDown"));
  printTab(2, 2);
  startTimer();
  if (flash.writeStr(stringAddress1, _string) && flash.powerDown() && !flash.writeStr(stringAddress2, _string))
    printPass();
  else
    printFail();

  writeTimer._pwrdwn = getTimer();
  printTab(3, 2);
  printTimer(writeTimer._pwrdwn);
  Serial.println();

  printTab(5, 0);
  Serial.print(F("powerUp"));
  printTab(3, 2);
  startTimer();
  if (flash.powerUp() && flash.writeStr(stringAddress3, _string))
    printPass();
  else
    printFail();

  writeTimer._pwrup = getTimer();
  printTab(3, 2);
  printTimer(writeTimer._pwrup);
  Serial.println();

  printTab(5, 0);
  Serial.print(F("sectorErase"));
  startTimer();
  printTab(2, 2);
  if (flash.eraseSector(stringAddress1) && flash.eraseSector(stringAddress2) && flash.eraseSector(stringAddress3))
    printPass();
  else
    printFail();

  writeTimer._Xsector = getTimer();
  printTab(3, 2);
  printTimer(writeTimer._Xsector);
  Serial.println();

  printTab(5, 0);
  Serial.print(F("chipErase"));
  printTab(2, 2);
  startTimer();
  if (flash.eraseChip())
    printPass();
  else
    printFail();

  writeTimer._Xchip = getTimer();
  printTab(3, 2);
  printTimer(writeTimer._Xchip);
  Serial.println();

  printLine();
}
