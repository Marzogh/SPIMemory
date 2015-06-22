/* Arduino SPIFlash Library v.1.2.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 21/06/2015
 *
 * This file is part of the Arduino SPIFlash Library. This library is for
 * W25Q80BV serial flash memory. In its current form it enables reading 
 * and writing bytes from and to various locations, reading and writing pages,
 * sector, block and chip erase, powering down for low power operation and
 * continuous read functions.
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
	uint32_t getID(void);
	bool     readBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer),
           readPage(uint16_t page_number, uint8_t *data_buffer),
           writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck = true),
           writeBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, bool errorCheck = true),
	         writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck = true),
	         eraseSector(uint16_t page_number),
	         eraseBlock32K(uint16_t page_number),
	         eraseBlock64K(uint16_t page_number),
	         eraseChip(void),
	         suspendProg(void),
	         resumeProg(void),
	         powerDown(void),
	         powerUp(void);
	void     printPage(uint16_t page_number, uint8_t outputType),
	         printAllPages(uint8_t outputType);
	uint8_t  readByte(uint16_t page_number, uint8_t offset);
  template <class T>  uint32_t writeAnything(uint16_t page_number, uint8_t offset, const T& value);
  template <class T> uint32_t readAnything(uint16_t page_number, uint8_t offset, T& value);


private:
	void     _chipSelect(void),
	         _chipDeselect(void),
	         _cmd(uint8_t c),
           _endProcess(void),
	         _empty(uint8_t *array),
	         _printPageBytes(uint8_t *data_buffer, uint8_t outputType);
	bool     _notBusy(uint32_t timeout = 100L),
           _beginRead(uint32_t address),
           _beginWrite(uint32_t address),
           _writeByte(uint32_t address, uint8_t data, bool errorCheck = true),
           _writeNextByte(uint8_t c),
		       _writeEnable(void),
		       _writeDisable(void),
	         _getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  uint8_t  _readNextByte(void),
           _addressCheck(uint32_t address);
	uint32_t _getAddress(uint16_t page_number, uint8_t offset = 0);
  
  volatile uint8_t *cs_port;
  uint8_t           cs_mask;
  uint8_t			chipSelect;
  bool			pageOverflow;
};

template <class T> uint32_t SPIFlash::writeAnything(uint16_t page_number, uint8_t offset, const T& value)
{
 uint32_t address = _getAddress(page_number, offset);
  const byte* p = (const byte*)(const void*)&value;
  //_beginWrite(address);
  uint16_t i;
  for(i = 0; i < sizeof(value);i++)
  {
    _writeByte(address++, *p++);
  }
  //_endProcess();
  return i;
}

template <class T> uint32_t SPIFlash::readAnything(uint16_t page_number, uint8_t offset, T& value)
{
  uint32_t address = _getAddress(page_number, offset);
  byte* p = (byte*)(void*)&value;
  _beginRead(address);
  uint16_t i;
  for(i = 0;i<sizeof(value);i++)
  {
    *p++ = _readNextByte();
  }
  _endProcess();
  return i;
}


#endif // _SPIFLASH_H_