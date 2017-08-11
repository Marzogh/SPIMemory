/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                          FlashDiagnostic_functions.ino                                                        |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 3.0.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  10.08.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
void printLine() {
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print(F("-"));
  }
  Serial.println();
}

void clearprintBuffer(char *bufPtr) {
  for (uint8_t i = 0; i < 128; i++) {
    //printBuffer[i] = 0;
    *bufPtr++ = 0;
  }
}

void printTime(uint32_t _wTime, uint32_t _rTime) {
  if (_rTime != 0) {
    Serial.print(F("\t\tWrite Time: "));
    printTimer(_wTime);
    Serial.print(F(",\tRead Time: "));
    printTimer(_rTime);
    Serial.println();
  }
  else {
    Serial.print(F("\t\tTime: "));
    printTimer(_wTime);
    Serial.println();
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
    Serial.print(_ms, 3);
    Serial.print(" ms");
  }
  else {
    Serial.print(_us);
    Serial.print(F(" us"));
  }
  delay(20);
}

void pass(bool _status) {
  Serial.print(F("\tData I/O test "));
  if (_status) {
    Serial.print(F("PASS\t"));
  }
  else {
    Serial.print(F("FAIL\t"));
  }
}

void getID() {
  char printBuffer[128];
  printLine();
  for (uint8_t i = 0; i < 68; i++) {
    Serial.print(F(" "));
  }
  Serial.print(F("SPIFlash Library version"));
#ifdef LIBVER
  uint8_t _ver, _subver, _bugfix;
  flash.libver(&_ver, &_subver, &_bugfix);
  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, ": %d.%d.%d", _ver, _subver, _bugfix);
  Serial.println(printBuffer);
#else
  Serial.println(F("< 2.5.0"));
#endif
  printLine();

  for (uint8_t i = 0; i < 80; i++) {
    Serial.print(F(" "));
  }
  Serial.println(F("Get ID"));
  printLine();
  uint8_t b1, b2;
  //uint16_t b3;
  uint32_t JEDEC = flash.getJEDECID();
  uint32_t maxPage = flash.getMaxPage();
  uint32_t capacity = flash.getCapacity();
  b1 = (JEDEC >> 16);
  b2 = (JEDEC >> 8);
  //b3 = (JEDEC >> 0);


  printLine();
  //---------------------------------------------------------------------------------------------//

  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, "\t\t\tJEDEC ID: %04xh", JEDEC);
  Serial.println(printBuffer);
  //Serial.print(F("\t\t\tJEDEC ID: "));
  //Serial.print(JEDEC, HEX);
  //Serial.println(F("xh"));
  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, "\t\t\tManufacturer ID: %02xh\n\t\t\tMemory Type: %02xh\n\t\t\tCapacity: %lu bytes\n\t\t\tMaximum pages: %lu", b1, b2, capacity, maxPage);
  Serial.println(printBuffer);
  printLine();
  Serial.println();
}

void byteTest() {
  uint32_t wTime, rTime, addr;
  uint8_t _data, _d;
  _d = 35;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeByte(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readByte(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tByte: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void charTest() {
  uint32_t wTime, rTime, addr;
  int8_t _data, _d;
  _d = -110;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeChar(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readChar(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tChar: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void wordTest() {
  uint32_t wTime, rTime, addr;
  uint16_t _data, _d;
  _d = 4520;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeWord(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readWord(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tWord: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void shortTest() {
  uint32_t wTime, rTime, addr;
  int16_t _data, _d;
  _d = -1250;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeShort(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readShort(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tShort: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void uLongTest() {
  uint32_t wTime, rTime, addr;
  uint32_t _data, _d;
  _d = 876532;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeULong(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readULong(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tULong: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void longTest() {
  uint32_t wTime, rTime, addr;
  int32_t _data, _d;
  _d = -10959;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeLong(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readLong(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tLong: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void floatTest() {
  uint32_t wTime, rTime, addr;
  float _data, _d;
  _d = 3.14;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeFloat(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  _data = flash.readFloat(addr);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tFloat: \t\t");
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void stringTest() {
  uint32_t wTime, rTime, addr;
  String _data, _d;
  _d = "This is a test String 123!@#";

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeStr(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  flash.readStr(addr, _data);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tString: \t");
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
    word s1;
    float s2;
    long s3;
    bool s4;
    byte s5;
  };
  Test _d;
  Test _data;

  _d.s1 = 31325;
  _d.s2 = 4.84;
  _d.s3 = 880932;
  _d.s4 = true;
  _d.s5 = 5;

  uint32_t addr, wTime, rTime;

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeAnything(addr, _d)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  flash.readAnything(addr, _data);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tStruct: \t");
  if (_d.s1 == _data.s1 && _d.s2 == _data.s2 && _d.s3 == _data.s3 && _d.s4 == _data.s4 && _d.s5 == _data.s5) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void arrayTest() {
  uint32_t wTime, rTime, addr;
  uint8_t _d[256], _data[256];

  for (uint16_t i = 0; i < 256; i++) {
    _d[i] = i;
  }

  addr = random(0, 0xFFFFF);
  wTime = micros();
  if (flash.writeByteArray(addr, _d, 256)) {
    wTime = micros() - wTime;
  }

  rTime = micros();
  flash.readByteArray(addr, _data, 256);
  rTime = micros() - rTime;


  Serial.print ("\t\t\tByte Array: \t");
  for (uint16_t i = 0; i < 256; i++) {
    if (_data[i] != i) {
      pass(FALSE);
      }
  }
  pass(TRUE);
  printTime(wTime, rTime);
  Serial.println();
}

void powerDownTest() {
  uint32_t _time;
  Serial.print(F("\t\t\tPower Down: \t"));
  _time = micros();
  if (flash.powerDown()) {
    _time = micros() - _time;
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }
}

void powerUpTest() {
  uint32_t _time;
  Serial.print(F("\t\t\tPower Up: \t"));
  _time = micros();
  if (flash.powerUp()) {
    _time = micros() - _time;
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
  Serial.print(F("\t\t\tErase 4KB Sector: "));
  _time = micros();
  if (flash.eraseSector(_addr)) {
    _time = micros() - _time;
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }
  Serial.println();
}

void eraseBlock32KTest() {
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  Serial.print(F("\t\t\tErase 32KB Block: "));
  _time = micros();
  if (flash.eraseBlock32K(_addr)) {
    _time = micros() - _time;
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
  Serial.print(F("\t\t\tErase 64KB Block: "));
  _time = micros();
  if (flash.eraseBlock64K(_addr)) {
    _time = micros() - _time;
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }
}

void eraseChipTest() {
  uint32_t _time;
  Serial.print(F("\t\t\tErase Chip: \t"));
  _time = micros();
  if (flash.eraseChip()) {
    _time = micros() - _time;
    pass(TRUE);
    printTime(_time, 0);
  }
  else {
    pass(FALSE);
  }
}

