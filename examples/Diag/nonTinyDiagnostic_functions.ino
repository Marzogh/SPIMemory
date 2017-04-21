
  //************************************************************************************************//
  //                                                                                                //
  //                                 If using a non ATTiny85 board                                  //
  //                                                                                                //
  //************************************************************************************************//
  #if !defined (__AVR_ATtiny85__)

void saveResults() {

#if !defined (__AVR_ATtiny85__)
  //************************************************************************************************//
  //                                                                                                //
  //                                 If using a non ATTiny85 board                                  //
  //                                                                                                //
  //************************************************************************************************//
  //Set prevWritten status to true, if not already so
  if (!prevWritten()) {
    setWrittenStatus();
  }
  addr = flash.getAddress(sizeof(dataPacket));
  //Write dataPacket to flash
  flash.writeAnything(addr, dataPacket);


#else
  //************************************************************************************************//
  //                                                                                                //
  //                                   If using an ATTiny85 board                                   //
  //                                                                                                //
  //************************************************************************************************//
  addr = flash.getAddress(sizeof(_status));
  flash.writeByte(addr, _status);
#endif
}

void byteDiag() {
  //Set variables
  byte _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = 35;

  //************************************************************************************************//
  //                                                                                                //
  //                                 If using a non ATTiny85 board                                  //
  //                                                                                                //
  //************************************************************************************************//
  //Test & time Write function
#if !defined (__AVR_ATtiny85__)
  startTime = micros();
  if (flash.writeByte(addr, _data)) {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += PASS << 0;
  }
  else
  {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += FAIL << 0;
  }

  //Test & time Read function
  startTime = micros();
  _d = flash.readByte(addr);
  dataPacket.rTime = micros() - startTime;
  if (_d == _data) {
    dataPacket.Status += PASS << 1;
  }
  else {
    dataPacket.Status += FAIL << 1;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);

  //Test & time Write function without Error check
  startTime = micros();
  if (flash.writeByte(addr, _data, NOERRCHK)) {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += PASS << 2;
  }
  else {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += FAIL << 2;
  }

#else
  //************************************************************************************************//
  //                                                                                                //
  //                                   If using an ATTiny85 board                                   //
  //                                                                                                //
  //************************************************************************************************//
  //Test & time Write function
  if (flash.writeByte(addr, _data)) {
    _status += PASS << 0;
  }
  else
  {
    _status += FAIL << 0;
  }


  //Test & time Read function
  _d = flash.readByte(addr);
  if (_d == _data) {
    _status += PASS << 1;
  }
  else {
    _status += FAIL << 1;
  }
#endif
  //************************************************************************************************//
  //                                                                                                //
  //                                    Non-board specific code                                     //
  //                                                                                                //
  //************************************************************************************************//
  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void charDiag() {
  //Set variables
  char _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = -110;

  //************************************************************************************************//
  //                                                                                                //
  //                                 If using a non ATTiny85 board                                  //
  //                                                                                                //
  //************************************************************************************************//
  //Test & time Write function
#if !defined (__AVR_ATtiny85__)
  startTime = micros();
  if (flash.writeChar(addr, _data)) {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += PASS << 0;
  }
  else
  {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += FAIL << 0;
  }

  //Test & time Read function
  startTime = micros();
  _d = flash.readChar(addr);
  dataPacket.rTime = micros() - startTime;
  if (_d == _data) {
    dataPacket.Status += PASS << 1;
  }
  else {
    dataPacket.Status += FAIL << 1;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);

  //Test & time Write function without Error check
  startTime = micros();
  if (flash.writeChar(addr, _data, NOERRCHK)) {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += PASS << 2;
  }
  else {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += FAIL << 2;
  }

#else
  //************************************************************************************************//
  //                                                                                                //
  //                                   If using an ATTiny85 board                                   //
  //                                                                                                //
  //************************************************************************************************//
  //Test & time Write function
  if (flash.writeChar(addr, _data)) {
    _status += PASS << 0;
  }
  else
  {
    _status += FAIL << 0;
  }


  //Test & time Read function
  _d = flash.readChar(addr);
  if (_d == _data) {
    _status += PASS << 1;
  }
  else {
    _status += FAIL << 1;
  }
#endif
  //************************************************************************************************//
  //                                                                                                //
  //                                    Non-board specific code                                     //
  //                                                                                                //
  //************************************************************************************************//
  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void wordDiag() {
  //Set variables
  word _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = 4520;

  //************************************************************************************************//
  //                                                                                                //
  //                                 If using a non ATTiny85 board                                  //
  //                                                                                                //
  //************************************************************************************************//
  //Test & time Write function
#if !defined (__AVR_ATtiny85__)
  startTime = micros();
  if (flash.writeWord(addr, _data)) {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += PASS << 0;
  }
  else
  {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += FAIL << 0;
  }

  //Test & time Read function
  startTime = micros();
  _d = flash.readWord(addr);
  dataPacket.rTime = micros() - startTime;
  if (_d == _data) {
    dataPacket.Status += PASS << 1;
  }
  else {
    dataPacket.Status += FAIL << 1;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);

  //Test & time Write function without Error check
  startTime = micros();
  if (flash.writeWord(addr, _data, NOERRCHK)) {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += PASS << 2;
  }
  else {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += FAIL << 2;
  }

#else
  //************************************************************************************************//
  //                                                                                                //
  //                                   If using an ATTiny85 board                                   //
  //                                                                                                //
  //************************************************************************************************//
  //Test & time Write function
  if (flash.writeWord(addr, _data)) {
    _status += PASS << 0;
  }
  else
  {
    _status += FAIL << 0;
  }


  //Test & time Read function
  _d = flash.readWord(addr);
  if (_d == _data) {
    _status += PASS << 1;
  }
  else {
    _status += FAIL << 1;
  }
#endif
  //************************************************************************************************//
  //                                                                                                //
  //                                    Non-board specific code                                     //
  //                                                                                                //
  //************************************************************************************************//
  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}


//************************************************************************************************//
//                                                                                                //
//                                 Only for a non ATTiny85 board                                  //
//                                                                                                //
//************************************************************************************************//
void shortDiag() {
  //Set variables
  short _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = -1250;
  //Test & time Write function
#if !defined (__AVR_ATtiny85__)
  startTime = micros();
  if (flash.writeShort(addr, _data)) {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += PASS << 0;
  }
  else
  {
    dataPacket.wTime = micros() - startTime;
    dataPacket.Status += FAIL << 0;
  }

  //Test & time Read function
  startTime = micros();
  _d = flash.readShort(addr);
  dataPacket.rTime = micros() - startTime;
  if (_d == _data) {
    dataPacket.Status += PASS << 1;
  }
  else {
    dataPacket.Status += FAIL << 1;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);

  //Test & time Write function without Error check
  startTime = micros();
  if (flash.writeShort(addr, _data, NOERRCHK)) {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += PASS << 2;
  }
  else {
    dataPacket.wNCTime = micros() - startTime;
    dataPacket.Status += FAIL << 2;
  }
  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
#endif
}

void structDiag(void) {
  struct Configuration {
    float lux;
    float vOut;                   // Voltage ouput from potential divider to Analog input
    float RLDR;                   // Resistance calculation of potential divider with LDR
    bool light;
    uint8_t adc;
  };
  Configuration _data;
  _data.lux = 98.43;
  _data.vOut = 4.84;
  _data.RLDR = 889.32;
  _data.light = true;
  _data.adc = 5;
#if (!prevWritten)
  flash.writeAnything(addr, _data);
  flash.readAnything(addr, _data);
  flash.eraseSector(addr);
#endif
}

void diagnose(void) {
  byteDiag();
  charDiag();
  wordDiag();
  shortDiag();
  uLongDiag();
  longDiag();
  floatDiag();
  stringDiag();
  structDiag();
  bArrayDiag();
  cArrayDiag();
}

#endif

