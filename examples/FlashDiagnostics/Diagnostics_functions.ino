/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                          FlashDiagnostic_functions.ino                                                        |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 3.1.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  04.03.2018                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                 uncomment #define RUNDIAGNOSTIC in SPIFlash.h in the library before compiling                                 |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
void printLine() {
  Serial.println();
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print("-");
  }
  Serial.println();
}

void printTab(uint8_t _times) {
  for (uint8_t i = 0; i < _times; i++) {
    Serial.print("\t");
  }
}

void clearprintBuffer(char *bufPtr) {
  for (uint8_t i = 0; i < 128; i++) {
    //printBuffer[i] = 0;
    *bufPtr++ = 0;
  }
}

void printTime(uint32_t _wTime, uint32_t _rTime) {
  if (_rTime != 0) {
    printTab(2);
    Serial.print("Write Time: ");
    printTimer(_wTime);
    Serial.print(F(","));
    printTab(1);
    Serial.print("Read Time: ");
    printTimer(_rTime);
  }
  else {
    printTab(2);
    Serial.print(F("Time: "));
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
  printTab(1);
  Serial.print("Data I/O test ");
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
  char printBuffer[128];
  printLine();
  for (uint8_t i = 0; i < 68; i++) {
    Serial.print(F(" "));
  }
  Serial.print("Version");
#ifdef LIBVER
  uint8_t _ver, _subver, _bugfix;
  flash.libver(&_ver, &_subver, &_bugfix);
  clearprintBuffer(&printBuffer[1]);
  sprintf(printBuffer, ": %d.%d.%d", _ver, _subver, _bugfix);
  Serial.print(printBuffer);
#else
  Serial.print(F("< 2.5.0"));
#endif
  printLine();

  for (uint8_t i = 0; i < 80; i++) {
    Serial.print(F(" "));
  }
  Serial.print(F("Get ID"));
  printLine();
  //uint8_t b1, b2;
  //uint16_t b3;
  //uint32_t JEDEC = flash.getJEDECID();
  //uint32_t maxPage = flash.getMaxPage();
  //uint32_t capacity = flash.getCapacity();
  //b1 = (JEDEC >> 16);
  //b2 = (JEDEC >> 8);
  //b3 = (JEDEC >> 0);

  clearprintBuffer(&printBuffer[1]);
  if (!flash.getJEDECID()) {
    Serial.println("No comms. Check wiring. Is chip supported? If unable to fix, raise an issue on Github");
    return false;
  }
  else {
    uint32_t JEDEC = flash.getJEDECID();
    printTab(3);
    Serial.print("JEDEC ID: 0x");
    Serial.println(JEDEC, HEX);
    Serial.print("Man ID: 0x");
    Serial.println(JEDEC >> 16);
    printTab(3);
    Serial.print("Memory ID: 0x");
    Serial.println(JEDEC >> 8);
    printTab(3);
    Serial.print("Capacity:");
    Serial.println(flash.getCapacity());
    printTab(3);
    Serial.print("Max Pages");
    Serial.println(flash.getMaxPage());
    printTab(3);
    printUniqueID();
    printLine();
    return true;
  }
}

void byteTest() {
  Serial.println();
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
  Serial.print ("Byte: ");
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
  Serial.println();
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
  Serial.print ("Char: ");
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
  Serial.println();
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
  Serial.print ("Word: ");
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
  Serial.println();
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
  Serial.print ("Short: ");
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
  Serial.println();
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
  Serial.print ("ULong: ");
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
  Serial.println();
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
  Serial.print ("Long: ");
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
  Serial.println();
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
  Serial.print ("Float: ");
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
  Serial.print ("String: ");
  printTab(1);
  if (_data == _d) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);

#if defined (ARDUINO_ARCH_SAM) || defined (ARDUINO_ARCH_ESP8266)
  Serial.println();
  printLine();
  if (!flash.functionRunTime()) {
    Serial.println(F("For function runtimes uncomment RUNDIAGNOSTIC in SPIMemory.h"));
  }
#endif
}

void structTest() {
  Serial.println();
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
  Serial.print ("Struct: ");
  printTab(1);
  if ((_d.s1 == _data.s1) && (_d.s2 == _data.s2) && (_d.s3 == _data.s3) && (_d.s4 == _data.s4) && (_d.s5 == _data.s5) && (_d.structofstruct.b1 == _data.structofstruct.b1) && (_d.structofstruct.b1 == _data.structofstruct.b1)) {
    pass(TRUE);
  }
  else {
    pass(FALSE);
  }
  printTime(wTime, rTime);
}

void arrayTest() {
  Serial.println();
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
  Serial.print ("Byte Array: ");
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
  Serial.println();
  uint32_t _time;
  printTab(3);
  Serial.print("Power Down:");
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
  Serial.println();
  uint32_t _time;
  printTab(3);
  Serial.print("Power Up: ");
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
  Serial.println();
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 4KB: ");
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
  Serial.println();
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 72KB: ");
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
  Serial.println();
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 32KB: ");
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
  Serial.println();
  uint32_t _time, _addr;
  _addr = random(0, 0xFFFFF);
  printTab(3);
  Serial.print("Erase 64KB: ");
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
  Serial.println();
  uint32_t _time;
  printTab(3);
  Serial.print("Erase Chip: ");
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

