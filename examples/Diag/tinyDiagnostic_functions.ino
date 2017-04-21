//************************************************************************************************//
//                                                                                                //
//                                   If using an ATTiny85 board                                   //
//                                                                                                //
//************************************************************************************************//
#if defined (__AVR_ATtiny85__)

void saveResults() {
  addr = flash.getAddress(sizeof(_status));
  flash.writeByte(addr, _status);
}

void _8bitDiag() {
  //Set variables
  byte _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = 35;

  //Test & time Write function
  if (flash.writeByte(addr, _data)) {
    _status += PASS;
  }
  else
  {
    _status += FAIL;
  }


  //Test & time Read function
  _d = flash.readByte(addr);
  if (_d == _data) {
    _status = (_status << 1) + PASS;
  }
  else {
    _status = (_status << 1) + FAIL;
  }
  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void _16bitDiag() {
  //Set variables
  word _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = 4520;

  //Test & time Write function
  if (flash.writeWord(addr, _data)) {
    _status += PASS;
  }
  else
  {
    _status += FAIL;
  }


  //Test & time Read function
  _d = flash.readWord(addr);
  if (_d == _data) {
    _status = (_status << 1) + PASS;
  }
  else {
    _status = (_status << 1) + FAIL;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void _32bitDiag() {
  //Set variables
  uint32_t _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = 876532;

  //Test & time Write function
  if (flash.writeULong(addr, _data)) {
    _status += PASS;
  }
  else
  {
    _status += FAIL;
  }


  //Test & time Read function
  _d = flash.readULong(addr);
  if (_d == _data) {
    _status = (_status << 1) + PASS;
  }
  else {
    _status = (_status << 1) + FAIL;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void floatDiag() {
  //Set variables
  float _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = 3.1412;

  //Test & time Write function
  if (flash.writeFloat(addr, _data)) {
    _status += PASS;
  }
  else
  {
    _status += FAIL;
  }


  //Test & time Read function
  _d = flash.readFloat(addr);
  if (_d == _data) {
    _status = (_status << 1) + PASS;
  }
  else {
    _status = (_status << 1) + FAIL;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void structDiag() {
  //Set variables
  struct Configuration {
    float lux;
    float vOut;                   // Voltage ouput from potential divider to Analog input
    float RLDR;                   // Resistance calculation of potential divider with LDR
    bool light;
    uint8_t adc;
  };
  Configuration _data, _d;
  _data.lux = 98.43;
  _data.vOut = 4.84;
  _data.RLDR = 889.32;
  _data.light = true;
  _data.adc = 5;
  addr = random(STARTADDR, 0xFFFF);

  //Test & time Write function
  if (flash.writeAnything(addr, _data)) {
    _status += PASS;
  }
  else
  {
    _status += FAIL;
  }

  //Test & time Read function
  if (flash.readAnything(addr, _d)) {
    if (_d.lux == _data.lux && _d.vOut == _data.vOut && _d.RLDR == _data.RLDR && _d.light == _data.light && _d.lux == _data.adc) {
      _status = (_status << 1) + PASS;
    }
    else {
      _status = (_status << 1) + FAIL;
    }
  }
  else {
    _status = (_status << 1) + FAIL;
  }

  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void stringDiag() {
  //Set variables
  String _data, _d;
  addr = random(STARTADDR, 0xFFFF);
  _data = "This is a test string";

  //Test & time Write function
  if (flash.writeStr(addr, _data)) {
    _status += PASS;
  }
  else
  {
    _status += FAIL;
  }


  //Test & time Read function
  if (flash.readStr(addr, _d)) {
    if (_d == _data) {
      _status = (_status << 1) + PASS;
    }
    else {
      _status = (_status << 1) + FAIL;
    }
  }
  else {
    _status = (_status << 1) + FAIL;
  }
  //Erase the sector previously written to
  flash.eraseSector(addr);
  saveResults();
}

void diagnose(void) {
  _8bitDiag();
  floatDiag();
  structDiag();
  stringDiag();
  //arrayDiag();
}

#endif

