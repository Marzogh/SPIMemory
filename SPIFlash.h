/* Arduino SPIFlash Library v.1.3.2
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 08/10/2015
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
	bool     readByte(uint16_t page_number, uint8_t offset, uint8_t data);
  bool     writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck = true);
  bool     writeBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, bool errorCheck = true);
  bool     writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck = true);
  bool     writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck = true);
  bool     writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck = true);
  bool     writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck = true);
  bool     writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck = true);
  bool     writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck = true);
  bool     writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck = true);
  bool     writeStr(uint16_t page, uint8_t offset, String &inputStr, bool errorCheck = true);
  bool     eraseSector(uint16_t page_number);
  bool     eraseBlock32K(uint16_t page_number);
  bool     eraseBlock64K(uint16_t page_number);
  bool     eraseChip(void);
  bool     suspendProg(void);
  bool     resumeProg(void);
  bool     powerDown(void);
  bool     powerUp(void);
  bool     readSerialStr(String &inputStr);
  bool     getAddress(uint16_t size, uint16_t &page_number, uint8_t &offset);
	void     begin();
  void     readBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer);
  void     readPage(uint16_t page_number, uint8_t *data_buffer);
  void     printPage(uint16_t page_number, uint8_t outputType);
  void     printAllPages(uint8_t outputType);
  void     readStr(uint16_t page, uint8_t offset, String &outStr);
	int8_t   readChar(uint16_t page_number, uint8_t offset);
  uint8_t  readByte(uint16_t page_number, uint8_t offset);
  int16_t  readShort(uint16_t page_number, uint8_t offset);
  uint16_t readWord(uint16_t page_number, uint8_t offset);
  int32_t  readLong(uint16_t page_number, uint8_t offset);
  uint32_t readULong(uint16_t page_number, uint8_t offset);
  float    readFloat(uint16_t page_number, uint8_t offset);
  template <class T> uint32_t writeAnything(uint16_t page_number, uint8_t offset, const T& value);
  template <class T> uint32_t readAnything(uint16_t page_number, uint8_t offset, T& value);


private:
	void     _chipSelect(void);
  void     _chipDeselect(void);
  void     _cmd(uint8_t c);
  void     _endProcess(void);
  void     _errorCodeCheck(void);
  void     _empty(uint8_t *array);
  void     _beginRead(uint32_t address);
  void     _printPageBytes(uint8_t *data_buffer, uint8_t outputType);
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
  uint8_t  _readByte(uint32_t address);
  uint32_t _getAddress(uint16_t page_number, uint8_t offset = 0);
  uint32_t _prepRead(uint16_t page_number, uint8_t offset = 0);
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
};

template <class T> uint32_t SPIFlash::writeAnything(uint16_t page_number, uint8_t offset, const T& value)
{
  uint32_t address = _getAddress(page_number, offset);
  Serial.println(address);
    const byte* p = (const byte*)(const void*)&value;
    uint16_t i;
    for (i = 0; i < sizeof(value); i++){
      _writeByte(address++, *p++);
      }
    return i;
}

template <class T> uint32_t SPIFlash::readAnything(uint16_t page_number, uint8_t offset, T& value)
{
  uint32_t address = _getAddress(page_number, offset);
  uint8_t errorcode;
  Serial.println(address);
    byte* p = (byte*)(void*)&value;
    uint16_t i;
    for (i = 0; i < sizeof(value); i++) {
      *p++ = _readByte(address++);
    }
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