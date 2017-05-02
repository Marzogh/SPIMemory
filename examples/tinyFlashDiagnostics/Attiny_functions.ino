/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                             Attiny_functions.ino                                                              |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.7.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  26.04.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#if defined (__AVR_ATtiny85__)

void startup(void) {
#if defined (CHIPSIZE)
  flash.begin(CHIPSIZE); //use flash.begin(CHIPSIZE) if using non-Winbond flash
#else
  flash.begin();
#endif
}

void setWrittenStatus(void) {
  dataPacket.writeStatus |= PASS;
  dataPacket.writeStatus |= ATTINY85;
}

void saveResults() {
  flash.writeAnything(addr, dataPacket);
}

void setTest(uint8_t _t) {
  dataPacket.test |= _t;
}

void intDiag(uint32_t _addr) {
  //Set variables
  word _data, _d;
  _data = 4520;
  setTest(INT);

  //Test & time Write function
  if (flash.writeWord(_addr, _data)) {
    dataPacket.Status |= iW;
  }
  else
  {
    dataPacket.Status &= !iW;
  }


  //Test & time Read function
  _d = flash.readWord(_addr);
  if (_d == _data) {
    dataPacket.Status |= iR;
  }
  else
  {
    dataPacket.Status &= !iR;
  }

  //Erase the sector previously written to
  flash.eraseSector(_addr);
}

void floatDiag(uint32_t _addr) {
  //Set variables
  float _data, _d;
  _data = 3.1412;
  setTest(FLOAT);

  //Test & time Write function
  if (flash.writeFloat(_addr, _data)) {
    dataPacket.Status |= fW;
  }
  else
  {
    dataPacket.Status &= !fW;
  }


  //Test & time Read function
  _d = flash.readFloat(_addr);
  if (_d == _data) {
    dataPacket.Status |= fR;
  }
  else
  {
    dataPacket.Status &= !fR;
  }

  //Erase the sector previously written to
  flash.eraseSector(_addr);
}

void structDiag(uint32_t _addr) {
  //Set variables
  struct Configuration {                  // Voltage ouput fR;om potential divider to Analog input
    float RLDR;                   // Resistance calculation of potential divider with LDR
    bool light;
    uint8_t adc;
  };
  Configuration _data, _d;
  _data.RLDR = 89.32;
  _data.light = true;
  _data.adc = 5;
  setTest(STRUCT);

  //Test & time Write function
  if (flash.writeAnything(_addr, _data)) {
    dataPacket.Status |= scW;
  }
  else
  {
    dataPacket.Status &= !scW;
  }

  //Test & time Read function
  if (flash.readAnything(_addr, _d)) {
    if (_d.RLDR == _data.RLDR && _d.light == _data.light && _d.adc == _data.adc) {
      dataPacket.Status |= scR;
    }
    else
    {
      dataPacket.Status &= !scR;
    }
  }
  else {
    dataPacket.Status &= !scR;
  }

  //Erase the sector previously written to
  flash.eraseSector(_addr);
}

void stringDiag(uint32_t _addr) {
  //Set variables
  String _d = "";
  String _data = "1Ab# D";
  setTest(STRING);

  //Test & time Write function
  if (flash.writeStr(_addr, _data)) {
    dataPacket.Status |= sgW;
  }
  else
  {
    dataPacket.Status &= !sgW;
  }


  //Test & time Read function
  if (flash.readStr(_addr, _d)) {
    if (_d == _data) {
      dataPacket.Status |= sgR;
    }
    else
    {
      dataPacket.Status &= !sgR;
    }
    //Erase the sector previously written to
    flash.eraseSector(_addr);
  }
}

void arrayDiag(uint32_t _addr) {
  //Set variables
  uint8_t _data[20], _d[20];
  setTest(ARRAY);

  for (uint8_t i = 0; i < 21; i++) {
    _data[i] = i;
  }

  //Test & time Write function
  if (flash.writeByteArray(_addr, _data, 20)) {
    dataPacket.Status |= aW;
  }
  else
  {
    dataPacket.Status &= !aW;
  }


  //Test & time Read function
  if (flash.readByteArray(_addr, _d, 20)) {
    for (uint8_t i = 0; i < 21; i++)
      if (_d[i] != _data[i]) {
        dataPacket.Status &= aR;
        break;
      }
    dataPacket.Status |= aR;
  }
  //Erase the sector previously written to
  flash.eraseSector(_addr);
}

void eraseDiag(uint32_t _addr) {
  setTest(ERASE);

  //Test & time eraseBlock32K function
  if (flash.eraseBlock32K(_addr)) {
    dataPacket.Status |= eB;
  }
  else
  {
    dataPacket.Status &= !eB;
  }

  //Test & time eraseChip function
  if (flash.eraseChip()) {
    dataPacket.Status |= eC;
  }
  else
  {
    dataPacket.Status &= !eC;
  }
}

void powerDiag(void) {
  setTest(ERASE);

  //Test & time powerDown function
  if (flash.powerDown()) {
    dataPacket.Status |= pOFF;
  }
  else
  {
    dataPacket.Status &= !pOFF;
  }

  //Test & time powerUp function
  if (flash.powerUp()) {
    dataPacket.Status |= pON;
  }
  else
  {
    dataPacket.Status &= !pON;
  }
}

void diagnose(void) {
  do {
    flash.eraseChip();
  } while (prevWritten());
  
  setWrittenStatus();
  
  uint32_t testaddr = random(STARTADDR, 0xFFFFF);
#if defined INTTEST
  intDiag(testaddr);
#endif
#if defined FLOATTEST
  floatDiag(testaddr);
#endif
#if defined STRUCTTEST
  structDiag(testaddr);
#endif
#if defined STRINGTEST
  stringDiag(testaddr);
#endif
#if defined ARRAYTEST
  arrayDiag(testaddr);
#endif
#if defined ERASETEST
  eraseDiag(testaddr);
#endif
#if defined POWERTEST
  powerDiag();
#endif
  saveResults();
}

#endif
