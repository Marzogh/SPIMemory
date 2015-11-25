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
  Serial.println(F("                                                           Get ID                                                                 "));
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
  Serial.print(F("                                                       Winbond "));
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
  sprintf(printBuffer, "\tJEDEC ID: %04lxh", JEDEC);
  Serial.println(printBuffer);
  clearprintBuffer();
  sprintf(printBuffer, "\tManufacturer ID: %02xh\n\tMemory Type: %02xh\n\tCapacity: %02xh\n\tMaximum pages: %lu", b1, b2, b3, maxPage);
  Serial.println(printBuffer);
}

void writeData() {
  printLine();
  Serial.println(F("                                                          Write Data                                                              "));
  printLine();

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
  Serial.println(F("\tData written without errors"));
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
  Serial.println(F("                                                          Data Check                                                              "));
  printLine();

  Serial.println(F("\tData Written\t||\t\tData Read\t\t||\t\tResult"));
  printLine();

  //Serial.print(F("\t"));
  printTab(1, 0);
  Serial.print(_byte);
  //Serial.print(F("\t\t||\t\t"));
  printTab(2, 2);
  Serial.print(flash.readByte(bytePage, byteOffset));
  //Serial.print(F("\t\t\t||\t\t"));;
  printTab(3, 2);
  if (_byte == flash.readByte(bytePage, byteOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_char);
  printTab(2, 2);
  Serial.print(flash.readChar(charPage, charOffset));
  printTab(3, 2);
  if (_char == flash.readChar(charPage, charOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_word);
  printTab(2, 2);
  Serial.print(flash.readWord(wordPage, wordOffset));
  printTab(3, 2);
  if (_word == flash.readWord(wordPage, wordOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_short);
  printTab(2, 2);
  Serial.print(flash.readShort(shortPage, shortOffset));
  printTab(3, 2);
  if (_short == flash.readShort(shortPage, shortOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_uLong);
  printTab(2, 2);
  Serial.print(flash.readULong(ULongPage, ULongOffset));
  printTab(3, 2);
  if (_uLong == flash.readULong(ULongPage, ULongOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_long);
  printTab(2, 2);
  Serial.print(flash.readLong(longPage, longOffset));
  printTab(3, 2);
  if (_long == flash.readLong(longPage, longOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_float, 4);
  printTab(2, 2);
  Serial.print(flash.readFloat(floatPage, floatOffset), 4);
  printTab(3, 2);
  if (_float == flash.readFloat(floatPage, floatOffset))
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(_string);
  Serial.print(F("\t||\t\t"));
  String outString;
  flash.readStr(stringPage, stringOffset, outString);
  Serial.print(outString);
  printTab(2, 2);
  if (_string == outString)
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(F("inputStruct"));
  printTab(1, 2);
  flash.readAnything(structPage, structOffset, outputStruct);
  Serial.print(F("outputStruct"));
  printTab(2, 2);
  if (inputStruct.s1 == outputStruct.s1 && inputStruct.s2 == outputStruct.s2 && inputStruct.s3 == outputStruct.s3 && inputStruct.s4 == outputStruct.s4 && inputStruct.s5 == outputStruct.s5)
    printPass();
  else
    printFail();

  printTab(1, 0);
  Serial.print(F("0 - 255"));
  printTab(2, 2);
  if (flash.readPage(page, pageOutputBuffer))
    Serial.print(F("0 - 255"));
  else
    Serial.print(F("Unknown"));
  printTab(3, 2);
  if (checkPage())
    printPass();
  else
    printFail();
}

void checkFunctions() {
  printLine();
  Serial.println(F("                                                     Check Other Functions                                                        "));
  printLine();
  Serial.println(F("\t\t\tFunction\t\t||\t\tResult"));
  printLine();

  uint32_t capacity = flash.getCapacity();
  if (!Serial)
    Serial.begin(115200);
  uint32_t stringAddress1 = random(0, capacity);
  uint32_t stringAddress2 = random(0, capacity);
  uint32_t stringAddress3 = random(0, capacity);

  printTab(3, 0);
  Serial.print(F("powerDown"));
  printTab(2, 2);
  if (flash.writeStr(stringAddress1, _string) && flash.powerDown() && !flash.writeStr(stringAddress2, _string))
    printPass();
  else
    printFail();

  printTab(3, 0);
  Serial.print(F("powerUp"));
  printTab(3, 2);
  if (flash.powerUp() && flash.writeStr(stringAddress3, _string))
    printPass();
  else
    printFail();

  printTab(3, 0);
  Serial.print(F("sectorErase"));
  printTab(2, 2);
  if (flash.eraseSector(stringAddress1) && flash.eraseSector(stringAddress2) && flash.eraseSector(stringAddress3))
    printPass();
  else
    printFail();

  printTab(3, 0);
  Serial.print(F("chipErase"));
  printTab(2, 2);
  if (flash.eraseChip())
    printPass();
  else
    printFail();

  printLine();
}
