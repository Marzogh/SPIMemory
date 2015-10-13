/* Arduino SPIFlash Library v.2.0.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 14/10/2015
 *
 * This file is part of the Arduino SPIFlash Library. This library is for
 * Winbond NOR flash memory modules. In its current form it enables reading 
 * and writing individual data variables, structs and arrays from and to various locations;
 * reading and writing pages; continuous read functions; sector, block and chip erase;
 * suspending and resuming programming/erase and powering down for low power operation.
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License v3.0
 * along with the Arduino SPIFlash Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#ifndef SPIFLASH_H
#define SPIFLASH_H

#include <Arduino.h>

class SPIFlash {
public:
  SPIFlash(uint8_t cs = 10, bool overflow = true);
  uint16_t getManID();
	uint32_t getJEDECID();
  bool     writeByte(uint32_t address, uint8_t data, bool errorCheck = true);
  bool     writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck = true);
  bool     writeBytes(uint32_t address, uint8_t *data_buffer, bool errorCheck = true);
  bool     writeBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, bool errorCheck = true);
  bool     writeChar(uint32_t address, int8_t data, bool errorCheck = true);
  bool     writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck = true);
  bool     writeShort(uint32_t address, int16_t data, bool errorCheck = true);
  bool     writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck = true);
  bool     writeWord(uint32_t address, uint16_t data, bool errorCheck = true);
  bool     writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck = true);
  bool     writeLong(uint32_t address, int32_t data, bool errorCheck = true);
  bool     writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck = true);
  bool     writeULong(uint32_t address, uint32_t data, bool errorCheck = true);
  bool     writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck = true);
  bool     writeFloat(uint32_t address, float data, bool errorCheck = true);
  bool     writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck = true);
  bool     writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck = true);
  bool     writeStr(uint32_t address, String &inputStr, bool errorCheck = true);
  bool     writeStr(uint16_t page, uint8_t offset, String &inputStr, bool errorCheck = true);
  bool     eraseSector(uint32_t address);
  bool     eraseSector(uint16_t page_number, uint8_t offset);
  bool     eraseBlock32K(uint32_t address);
  bool     eraseBlock32K(uint16_t page_number, uint8_t offset);
  bool     eraseBlock64K(uint32_t address);
  bool     eraseBlock64K(uint16_t page_number, uint8_t offset);
  bool     eraseChip(void);
  bool     suspendProg(void);
  bool     resumeProg(void);
  bool     powerDown(void);
  bool     powerUp(void);
  bool     readSerialStr(String &inputStr);
  bool     getAddress(uint16_t size, uint16_t &page_number, uint8_t &offset);
  uint32_t getAddress(uint16_t size);
  uint32_t getCapacity();
  uint32_t getMaxPage();
	void     begin();
  void     readBytes(uint32_t address, uint8_t *data_buffer, bool fastRead = false);
  void     readBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, bool fastRead = false);
  void     readPage(uint16_t page_number, uint8_t *data_buffer, bool fastRead = false);
  void     readStr(uint16_t page, uint8_t offset, String &outStr, bool fastRead = false);
  void     readStr(uint32_t address, String &outStr, bool fastRead = false);
  int8_t   readChar(uint32_t address, bool fastRead = false);
	int8_t   readChar(uint16_t page_number, uint8_t offset, bool fastRead = false);
  uint8_t  readByte(uint16_t page_number, uint8_t offset, bool fastRead = false);
  uint8_t  readByte(uint32_t address, bool fastRead = false);
  int16_t  readShort(uint16_t page_number, uint8_t offset, bool fastRead = false);
  int16_t  readShort(uint32_t address, bool fastRead = false);
  uint16_t readWord(uint16_t page_number, uint8_t offset, bool fastRead = false);
  uint16_t readWord(uint32_t address, bool fastRead = false);
  int32_t  readLong(uint16_t page_number, uint8_t offset, bool fastRead = false);
  int32_t  readLong(uint32_t address, bool fastRead = false);
  uint32_t readULong(uint16_t page_number, uint8_t offset, bool fastRead = false);
  uint32_t readULong(uint32_t address, bool fastRead = false);
  float    readFloat(uint16_t page_number, uint8_t offset, bool fastRead = false);
  float    readFloat(uint32_t address, bool fastRead = false);
  template <class T> bool writeAnything(uint16_t page_number, uint8_t offset, const T& value, bool errorCheck = true);
  template <class T> bool writeAnything(uint32_t address, const T& value, bool errorCheck = true);
  template <class T> uint32_t readAnything(uint32_t address, T& value, bool fastRead = false);
  template <class T> uint32_t readAnything(uint16_t page_number, uint8_t offset, T& value, bool fastRead = false);


private:
  void     _cmd(uint8_t c);
  void     _endProcess(void);
  void     _errorCodeCheck(void);
  void     _empty(uint8_t *array);
  void     _beginRead(uint32_t address);
  void     _beginFastRead(uint32_t address);
	bool     _notBusy(uint32_t timeout = 10L);
  bool     _addressCheck(uint32_t address);
  bool     _beginWrite(uint32_t address);
  bool     _readPage(uint16_t page_number, uint8_t *page_buffer);
  bool     _writeNextByte(uint8_t c);
  bool     _writeEnable(void);
  bool     _writeDisable(void);
  bool     _getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  bool     _getManId(uint8_t *b1, uint8_t *b2);
  bool     _writeByte(uint32_t address, uint8_t data, bool errorCheck = true);
  uint8_t  _readNextByte(void);
  uint8_t  _readByte(uint32_t address, bool fastRead = false);
  uint32_t _getAddress(uint16_t page_number, uint8_t offset = 0);
  bool _prepRead(uint32_t address);
  uint32_t _prepRead(uint16_t page_number, uint8_t offset = 0);
  bool _prepWrite(uint32_t address);
  uint32_t _prepWrite(uint16_t page_number, uint8_t offset = 0);
  template <class T> bool _writeErrorCheck(uint32_t address, const T& value);
  
  volatile uint8_t *cs_port;
  uint8_t           cs_mask;
  uint8_t			chipSelect;
  uint32_t    capacity, maxPage, errorcode, currentAddress;
  bool			  pageOverflow;
  const uint8_t devType[10]   = {0x5, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  const uint32_t memSize[10]  = {64L * 1024L, 128L * 1024L, 256L * 1024L, 512L * 1024L, 1L * 1024L * 1024L,
                                2L * 1024L * 1024L, 4L * 1024L * 1024L, 8L * 1024L * 1024L, 16L * 1024L * 1024L};
  /*const uint8_t pinMap[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
  const uint8_t portMap[3] = {}*/
};

// Writes any type of data to a specific location in the flash memory. 
// Has two variants:
//  A. Takes two arguments - 
//    1. address --> Any address from 0 to maxAddress
//    2. T& value --> Variable to return data into
//    4. errorCheck --> Turned on by default. Checks for writing errors
//  B. Takes three arguments -
//    1. page --> Any page number from 0 to maxPage
//    2. offset --> Any offset within the page - from 0 to 255
//    3. const T& value --> Variable with the data to be written
//    4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
//      Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
template <class T> bool SPIFlash::writeAnything(uint32_t address, const T& value, bool errorCheck )
{  
#ifdef DIAGNOSTIC
  Serial.println(address);
#endif

  if (!_prepWrite(address))
    return false;
  else {
    const byte* p = (const byte*)(const void*)&value;
    _beginWrite(address);
    uint16_t i;
    for (i = 0; i < sizeof(value); i++){
      _writeNextByte(*p++);
    }
    _endProcess();
  }

  if (!errorCheck)
    return true;
  else {
    return _writeErrorCheck(address, value);
  }

  return false;
}
// Variant B
template <class T> bool SPIFlash::writeAnything(uint16_t page_number, uint8_t offset, const T& value, bool errorCheck )
{
  uint32_t address = _prepWrite(page_number, offset);
  
#ifdef DIAGNOSTIC
  Serial.println(address);
#endif

  const byte* p = (const byte*)(const void*)&value;
  _beginWrite(address);
  uint16_t i;
  for (i = 0; i < sizeof(value); i++){
    _writeNextByte(*p++);
  }
  _endProcess();

  if (!errorCheck)
    return true;
  else {
    return _writeErrorCheck(address, value);
  }

  return false;
}

// Reads any type of data from a specific location in the flash memory. 
// Has two variants:
//  A. Takes two arguments - 
//    1. address --> Any address from 0 to maxAddress
//    2. T& value --> Variable to return data into
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//  B. Takes three arguments -
//    1. page --> Any page number from 0 to maxPage
//    2. offset --> Any offset within the page - from 0 to 255
//    3. T& value --> Variable to return data into
//    3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
template <class T> uint32_t SPIFlash::readAnything(uint32_t address, T& value, bool fastRead)
{
  uint8_t errorcode;
  
#ifdef DIAGNOSTIC
  Serial.println(address);
#endif

  if (!_prepRead(address))
    return false;
  else {
    byte* p = (byte*)(void*)&value;
    uint16_t i;
    for (i = 0; i < sizeof(value); i++) {
      *p++ = _readByte(address++, fastRead);
    }
    return i;
  }
}
// Variant B
template <class T> uint32_t SPIFlash::readAnything(uint16_t page_number, uint8_t offset, T& value, bool fastRead)
{
  uint32_t address = _prepRead(page_number, offset);
  uint8_t errorcode;

#ifdef DIAGNOSTIC
  Serial.println(address);
#endif

  byte* p = (byte*)(void*)&value;
  uint16_t i;
    
  if(!fastRead)
    _beginRead(address);
  else
    _beginFastRead(address);

  for (i = 0; i < sizeof(value); i++) {
    *p++ = _readNextByte();
    address++;
  }
  _endProcess();
  return i;
}

template <class T> bool SPIFlash::_writeErrorCheck(uint32_t address, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  uint16_t i = 0;
  _beginRead(address);
  for(i = 0; i < (sizeof(value));i++)
  {
    if (*p++ != _readNextByte())
      return false;
  }
  _endProcess();
  return true;
}


#endif // _SPIFLASH_H_