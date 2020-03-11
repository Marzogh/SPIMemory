/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                          FlashDiagnostic_functions.ino                                                        |
  |                                                               SPIMemory library                                                                |
  |                                                                   v 3.2.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  02.05.2018                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                 uncomment #define RUNDIAGNOSTIC in SPIMemory.h in the library before compiling                                 |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
void printLine() {
  Serial.println();
  for (uint8_t i = 0; i < 125; i++) {
    Serial.print("-");
  }
  Serial.println();
}

void printTab(uint8_t _times) {
  for (uint8_t i = 0; i < _times; i++) {
    Serial.print("\t");
  }
}

void printTime(uint32_t _wTime, uint32_t _rTime) {
  printTab(2);
  if (_rTime != 0) {
    printTimer(_wTime);
    printTab(3);
    printTimer(_rTime);
  }
  else {
    printTab(1);
    Serial.print("      ");
    printTimer(_wTime);
  }
}

void printTimer(uint32_t _us) {

  if (_us > 1000000) {
    float _s = _us / (float)1000000;
    Serial.print(_s, 3);
    Serial.print(" s");
  }
  else if (_us > 1000) {
    float _ms = _us / (float)1000;
    Serial.print(_ms, 2);
    Serial.print(" ms");
  }
  else {
    Serial.print(_us);
    Serial.print(F(" us"));
  }
  delay(20);
}

void pass(bool _status) {
  printTab(1);
  Serial.print("   ");
  if (_status) {
    Serial.print("PASS");
  }
  else {
    Serial.print("FAIL");
  }
  printTab(1);
}

void printUniqueID(void) {
  long long _uniqueID = flash.getUniqueID();
  if (_uniqueID) {
    Serial.print("Unique ID: ");
    Serial.print(uint32_t(_uniqueID / 1000000L));
    Serial.print(uint32_t(_uniqueID % 1000000L));
    Serial.print(", ");
    Serial.print("0x");
    Serial.print(uint32_t(_uniqueID >> 32), HEX);
    Serial.print(uint32_t(_uniqueID), HEX);
  }
}

bool getID() {
  Serial.println();
  Serial.print("SPIMemory Library version: ");
#ifdef LIBVER
  uint8_t _ver, _subver, _bugfix;
  flash.libver(&_ver, &_subver, &_bugfix);
  Serial.print(_ver);
  Serial.print(".");
  Serial.print(_subver);
  Serial.print(".");
  Serial.println(_bugfix);
#else
  Serial.println(F("< 2.5.0"));
#endif
  Serial.println();
  uint32_t JEDEC = flash.getJEDECID();
  if (!JEDEC) {
    Serial.println("No comms. Check wiring. Is chip supported? If unable to fix, raise an issue on Github");
    return false;
  }
  else {
    Serial.print("JEDEC ID: 0x");
    Serial.println(JEDEC, HEX);
    Serial.print("Man ID: 0x");
    Serial.println(uint8_t(JEDEC >> 16), HEX);
    Serial.print("Memory ID: 0x");
    Serial.println(uint8_t(JEDEC >> 8), HEX);
    Serial.print("Capacity: ");
    Serial.println(flash.getCapacity());
    Serial.print("Max Pages: ");
    Serial.println(flash.getMaxPage());
    printUniqueID();

  }
  return true;
}

void byteTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  uint8_t _data, _d;
  _d = 35;

  addr = random(0, 0xFFFFF);

  if (flash.writeByte(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readByte(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Byte");
  printTab(2);

  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void charTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  int8_t _data, _d;
  _d = -110;

  addr = random(0, 0xFFFFF);

  if (flash.writeChar(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readChar(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Char");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void wordTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  uint16_t _data, _d;
  _d = 4520;

  addr = random(0, 0xFFFFF);

  if (flash.writeWord(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readWord(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Word");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void shortTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  int16_t _data, _d;
  _d = -1250;

  addr = random(0, 0xFFFFF);

  if (flash.writeShort(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readShort(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Short");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void uLongTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  uint32_t _data, _d;
  _d = 876532;

  addr = random(0, 0xFFFFF);

  if (flash.writeULong(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readULong(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("ULong");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void longTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  int32_t _data, _d;
  _d = -10959;

  addr = random(0, 0xFFFFF);

  if (flash.writeLong(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readLong(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Long");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void floatTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  float _data, _d;
  _d = 3.14;

  addr = random(0, 0xFFFFF);

  if (flash.writeFloat(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  _data = flash.readFloat(addr);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Float");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void stringTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  String _data, _d;
  _d = "This is a test String 123!@#";

  addr = random(0, 0xFFFFF);

  if (flash.writeStr(addr, _d)) {
    wTime = flash.functionRunTime();
  }

  flash.readStr(addr, _data);
  rTime = flash.functionRunTime();


  printTab(3);
  Serial.print ("String");
  printTab(2);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void structTest() {
  struct Test {
    uint16_t s1;
    float s2;
    int32_t s3;
    bool s4;
    uint8_t s5;
    struct structOfStruct {
      uint8_t b1;
      float f2;
    } structofstruct;
  };
  Test _d;
  Test _data;

  _d.s1 = 31325;
  _d.s2 = 4.84;
  _d.s3 = 880932;
  _d.s4 = true;
  _d.s5 = 5;
  _d.structofstruct.b1 = 234;
  _d.structofstruct.f2 = 6.28;

  uint32_t wTime = 0;
  uint32_t addr, rTime;

  addr = random(0, 0xFFFFF);

  if (flash.writeAnything(addr, _d)) {
    wTime = flash.functionRunTime();
  }


  flash.readAnything(addr, _data);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Struct");
  printTab(2);
  if ((_d.s1 == _data.s1) && (_d.s2 == _data.s2) && (_d.s3 == _data.s3) && (_d.s4 == _data.s4) && (_d.s5 == _data.s5) && (_d.structofstruct.b1 == _data.structofstruct.b1) && (_d.structofstruct.b1 == _data.structofstruct.b1)) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

}

void arrayTest() {
  uint32_t wTime = 0;
  uint32_t rTime, addr;
  uint8_t _d[256], _data[256];

  for (uint16_t i = 0; i < 256; i++) {
    _d[i] = i;
  }

  addr = random(0, 0xFFFFF);

  if (flash.writeByteArray(addr, _d, 256)) {
    wTime = flash.functionRunTime();
  }


  flash.readByteArray(addr, _data, 256);
  rTime = flash.functionRunTime();

  printTab(3);
  Serial.print ("Byte Array");
  printTab(1);
  for (uint16_t i = 0; i < 256; i++) {
    if (_data[i] != i) {
      pass(FALSE);
      Serial.print(i);
      Serial.print(", ");
    }
  }
  pass(TRUE);
  printTime(wTime, rTime);

}

void powerDownTest() {
  uint32_t _time;
  printTab(3);
  Serial.print("Power Down");
  printTab(1);
  if (flash.powerDown()) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
    printTab(2);
    Serial.print("Not all chips support power down. Check your datasheet.");
  }

}

void powerUpTest() {
  uint32_t _time;
  printTab(3);
  Serial.print("Power Up");
  printTab(1);
  if (flash.powerUp()) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }

}

void eraseSectorTest() {
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 4KB");
  printTab(1);
  if (flash.eraseSector(_addr)) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }

}

void eraseSectionTest() {
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 72KB");
  printTab(1);
  if (flash.eraseSection(_addr, KB(72))) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }

}

void eraseBlock32KTest() {
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 32KB");
  printTab(1);
  if (flash.eraseBlock32K(_addr)) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }

}

void eraseBlock64KTest() {
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 64KB");
  printTab(1);
  if (flash.eraseBlock64K(_addr)) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }

}

void eraseChipTest() {
  uint32_t _time;
  printTab(3);
  Serial.print("Erase Chip");
  printTab(1);
  if (flash.eraseChip()) {
    _time = flash.functionRunTime();
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }

}
