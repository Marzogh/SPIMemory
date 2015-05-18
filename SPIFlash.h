/* Arduino SPIFlash Library
 * Copyright (C) 2015 by Prajwal Bhattaram
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
  SPIFlash(uint8_t cs = 10);
	uint32_t getID(void);
	boolean  writeByte(uint16_t page, uint8_t offset, uint8_t data),
	         writePage(uint16_t page_number, uint8_t *page_buffer),
	         eraseSector(uint32_t address),
	         eraseBlock32K(uint32_t address),
	         eraseBlock64K(uint32_t address),
	         eraseChip(void),
	         suspendProg(void),
	         resumeProg(void),
	         powerDown(void),
	         powerUp(void),
	         beginRead(uint32_t address);
	void     endRead(void),
	         readPage(uint16_t page_number, uint8_t *page_buffer),
	         readAllPages(void);
	uint8_t  readNextByte(void),
	         readByte(uint16_t page, uint8_t offset);
	         
	         
	         
private:
	void     _cmd(uint8_t c),
	         _printPageBytes(uint8_t *page_buffer);
	boolean  _notBusy(uint32_t timeout = 100L),
		     _writeEnable(void),
		     _writeDisable(void),
		     _readPage(uint16_t page_number, uint8_t *page_buffer),
	         _writePage(uint16_t page_number, uint8_t *page_buffer),
	         _getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  
  volatile uint8_t *cs_port;
  uint8_t           cs_mask;
};

#endif // _SPIFLASH_H_