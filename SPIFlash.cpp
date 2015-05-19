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
#include "SPIFlash.h"
#include <util/delay.h>
#include <SPI.h>

#define PAGEPROG     0x02
#define READDATA     0x03
#define WRITEDISABLE 0x04
#define READSTAT1    0x05
#define WRITEENABLE  0x06
#define SECTORERASE  0x20
#define BLOCK32ERASE 0x52
#define CHIPERASE    0x60
#define SUSPEND      0x75
#define ID           0x90
#define RESUME       0x7A
#define JEDECID      0x9f
#define RELEASE      0xAB
#define POWERDOWN    0xB9
#define BLOCK64ERASE 0xD8

#define BUSY         0x01
#define WRTEN        0x02

// Currently library works only with W25Q80BV
#define CAPACITY       1L * 1024L * 1024L

//#define CHIP_SELECT   *cs_port |= ~cs_mask;
//#define CHIP_DESELECT *cs_port &=  cs_mask;
#define xfer(n)   SPI.transfer(n)

// Constructor
SPIFlash::SPIFlash(uint8_t cs) {
	//cs_mask = digitalPinToBitMask(cs);
	SPI.begin();
    SPI.setDataMode(0);
    SPI.setBitOrder(MSBFIRST);
    chipSelect = cs;
    pinMode(cs, OUTPUT);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Private functions used by read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Selects chip
void SPIFlash::_chipSelect() {
	digitalWrite(chipSelect, HIGH);
	digitalWrite(chipSelect, LOW);
}

//Deselects chip
void SPIFlash::_chipDeselect() {
	digitalWrite(chipSelect, HIGH);
}

// Select chip and issue command - data to follow
void SPIFlash::_cmd(uint8_t c) {
	_chipSelect();
	(void)xfer(c);
}

// Polls the status register 1 until busy flag is cleared or timeout
boolean SPIFlash::_notBusy(uint32_t timeout) {
	uint8_t state;
	uint32_t startTime = millis();

	do {
		_cmd(READSTAT1);
		state = xfer(0);
		_chipDeselect();
		if((millis()-startTime) > timeout)
			return false;
	} while(state & BUSY);
	return true;
}

//Enables writing to chip by setting the WRITEENABLE bit
boolean SPIFlash::_writeEnable(void) {
	uint8_t state;

	_cmd(WRITEENABLE);
	_chipDeselect();

	//verifies that WRITE is enabled
	_cmd(READSTAT1);
	state = xfer(0);
	_chipDeselect();
	return (state & WRTEN) ? true : false;
}

//Disables writing to chip by setting the Write Enable Latch (WEL) bit in the Status Register to 0
//_writeDisable() is not required under the following conditions because the Write Enable Latch (WEL) flag is cleared to 0
// i.e. to write disable state:
// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register,
// Erase Security Register and Program Security register
boolean SPIFlash::_writeDisable(void) {
	_cmd(WRITEDISABLE);
	_chipDeselect();
}


// Checks for presence of chip by requesting JEDEC ID
boolean SPIFlash::_getJedecId (byte *b1, byte *b2, byte *b3) {
  _cmd(JEDECID);
  *b1 = xfer(0); // manufacturer id
  *b2 = xfer(0); // memory type
  *b3 = xfer(0); // capacity
  _chipDeselect();
  if (!_notBusy())
  	return false;
  return true;
}  

//Reads a page of data into a page buffer
boolean  SPIFlash::_readPage(uint16_t page_number, uint8_t *page_buffer) {
	 if(!_notBusy())
	 	return false;

	 _cmd(READDATA);
	 // Construct the 24-bit address from the 16-bit page number and 0x00, since we will read 256 bytes (one page).
	 xfer((page_number >>  8) & 0xFF);
     xfer((page_number >>  0) & 0xFF);
  	 xfer(0);
	 for (int a = 0; a < 256; ++a) {
	 	page_buffer[a] = xfer(0);
	 }
	 _chipDeselect();

	 return true;
}

boolean SPIFlash::_writePage(uint16_t page_number, uint8_t *page_buffer) {
	if(!_notBusy() || page_number >= 4096 || !_writeEnable())
		return false;

	_cmd(PAGEPROG);
	// Construct the 24-bit address from the 16-bit page number and 0x00, since we will read 256 bytes (one page).
	xfer((page_number >>  8) & 0xFF);
	xfer((page_number >>  0) & 0xFF);
	xfer(0);
	for (int a = 0; a < 256; ++a) {
		xfer(page_buffer[a]);
	}
	_chipDeselect(); 
	delay(3);     // Maximum page program time - Ref. datasheet

	if(!_notBusy())
		return false;

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, ``Page Program``, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Prints properly formatted data from page reads - for debugging
void SPIFlash::_printPageBytes(uint8_t *page_buffer) {
	char buffer[10];
	for (int a = 0; a < 16; ++a) {
		for (int b = 0; b < 16; ++b) {
			sprintf(buffer, "%02x", page_buffer[a * 16 + b]);
			Serial.print(buffer);
		}
		Serial.println();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Public functions used for read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Checks for and initiates the chip by requesting JEDEC ID which is returned as a 32 bit int
uint32_t SPIFlash::getID(void) {

    byte b1, b2, b3;
    _getJedecId(&b1, &b2, &b3);
      char buffer[128];
      sprintf(buffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh", b1, b2, b3);
      Serial.println(buffer);
}


//Initiates read operation - but data is not read yet
boolean SPIFlash::beginRead(uint32_t address) {
	if((address >= CAPACITY) || !_notBusy())
		return false;

	_cmd(READDATA);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);

	//SPI data lines are left open until endRead() is called

	return true;
}

//Reads next byte. Call 'n' times to read 'n' number of bytes. Should be called after beginRead()
uint8_t SPIFlash::readNextByte(void) {
	return xfer(0);
}

//Stops read operation. Should be called after all the required data is read from repeated readNextByte() calls
void SPIFlash::endRead(void) {
	_chipDeselect();
}

//Reads a byte of data from a specific location in a page. Takes two arguements -
// 1. page --> Any page number from 0 to 4095
// 2. offset --> Any offset within the page - from 0 to 255
uint8_t SPIFlash::readByte(uint16_t page, uint8_t offset) {
	if(_notBusy()) {
		uint8_t page_data[256];
		_readPage(page, page_data);
		return page_data[offset];
	}
}

//Writes a byte of data to a specific location in a page. Takes three arguements -
// 1. page --> Any page number from 0 to 4095
// 2. offset --> Any offset within the page - from 0 to 255
// 3. data --> One byte of data to be written to a particular location on a page
boolean SPIFlash::writeByte(uint16_t page, uint8_t offset, uint8_t data) {
	if(!_notBusy())
		return false;

	uint8_t page_data[256];
	_readPage(page, page_data);
	page_data[offset] = data;
	_writePage(page, page_data);

}

//Writes a page of data from a page_buffer array. Make sure the sizeOf(uint8_t page_buffer[]) == 256. 
boolean SPIFlash::writePage(uint16_t page_number, uint8_t *page_buffer) {
	uint8_t temp_buffer[256];
	char buffer[80];
	
	_writePage(page_number, page_buffer);
	_readPage(page_number, temp_buffer);

	for(int a=0; a<256; ++a) {
		if(!temp_buffer[a] == page_buffer[a])
		return false;
}
	sprintf(buffer, "Writing page (%04x) done", page_number);
	Serial.println(buffer);
	return true;
}

//Erases one 4k sector
boolean SPIFlash::eraseSector(uint32_t address) {
	if(!_notBusy() || !_writeEnable())
		return false;

	_cmd(SECTORERASE);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	_chipDeselect();

	if(!_notBusy(1000L))
		return false;	//Datasheet says erasing a sector takes 400ms max

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, ``Sector Erase``, Block Erase, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Erases one 32k sector
boolean SPIFlash::eraseBlock32K(uint32_t address) {
	if(!_notBusy() || !_writeEnable())
		return false;

	_cmd(BLOCK32ERASE);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	_chipDeselect();

	if(!_notBusy(1000L))
		return false;	//Datasheet says erasing a sector takes 400ms max

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Erases one 64k sector
boolean SPIFlash::eraseBlock64K(uint32_t address) {
	if(!_notBusy() || !_writeEnable())
		return false;

	_cmd(BLOCK64ERASE);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	_chipDeselect();

	if(!_notBusy(1000L))
		return false;	//Datasheet says erasing a sector takes 400ms max

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Erases whole chip. Think twice before using.
boolean SPIFlash::eraseChip(void) {
	if(!_notBusy() || !_writeEnable())
		return false;

	_cmd(CHIPERASE);
	_chipDeselect();

	if(!_notBusy(10000L))
		return false; //Datasheet says erasing chip takes 6s max

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, ``Chip Erase``, Write Status Register,
	// Erase Security Register and Program Security register

	return true;

}

//Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase().
//Page Program, Write Status Register, Erase instructions are not allowed.
//Erase suspend is only allowed during Block/Sector erase. 
//Program suspend is only allowed during Page/Quad Page Program
boolean SPIFlash::suspendProg(void) {
	if(_notBusy())
		return false;

	_cmd(SUSPEND);
	_chipDeselect();

	delay(20);			//Max suspend enable time according to the Datasheet

	if(!_notBusy())
		return false;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
boolean SPIFlash::resumeProg(void) {
	if(!_notBusy())
		return false;

	_cmd(RESUME);
	_chipDeselect();

	if(_notBusy())
		return false;

	return true;

}

//Puts device in low power state. Good for battery powered operations.
//Typical current consumption during power-down is 1mA with a maximum of 5mA. (Datasheet 7.4)
//In powerDown() the chip will only respond to powerUp()
boolean SPIFlash::powerDown(void) {
	if(!_notBusy())
		return false;

	_cmd(POWERDOWN);
	_chipDeselect();

	delay(3);							//Max powerDown enable time according to the Datasheet

	if(_writeEnable())					//Tries to read STAT1. If STAT1 is accessible, chip has not powered down
	{
		_writeDisable();
		return false;
	}

	return true;
}

//Wakes chip from low power state.
boolean SPIFlash::powerUp(void) {
	if(!_notBusy())
		return false;

	_cmd(RELEASE);						
	_chipDeselect();

	delay(3);						    //Max release enable time according to the Datasheet

	if(_writeEnable())					//Tries to read STAT1. If STAT1 is accessible, chip has powered up
	{
		_writeDisable();
		return true;
	}

	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//         Debugging code that returns data to Serial stream.         //
//         Declares Serial.begin() if not previously declared.        //
//                 Initiates Serial at 9600 baud.                     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Reads a page of data and prints it to Serial stream. Make sure the sizeOf(uint8_t page_buffer[]) == 256.
void SPIFlash::readPage(uint16_t page_number, uint8_t *page_buffer) {
	if(!Serial)
		Serial.begin(9600);

	char buffer[80];
	sprintf(buffer, "Reading page (%04x)", page_number);
	Serial.println(buffer);

	_readPage(page_number, page_buffer);
	_printPageBytes(page_buffer);
}

//Reads all pages on Flash chip and dumps it to Serial stream. 
//This function is useful when extracting data from a flash chip onto a computer as a text file.
void SPIFlash::readAllPages(void) {
	if(!Serial)
		Serial.begin(9600);

	Serial.println("Reading all pages");
	byte page_buffer[256];

	for (int a = 0; a < 4096; ++a) {
		_readPage(a, page_buffer);
		_printPageBytes(page_buffer);
  }
}
