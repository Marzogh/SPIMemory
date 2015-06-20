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

#define arrayLen(x)  (sizeof(x) / sizeof(*x))

// Currently library works only with W25Q80BV
#define CAPACITY       1L * 1024L * 1024L

//#define CHIP_SELECT   *cs_port |= ~cs_mask;
//#define CHIP_DESELECT *cs_port &=  cs_mask;
#define xfer(n)   SPI.transfer(n)

// Constructor
SPIFlash::SPIFlash(uint8_t cs, bool overflow) {
	//cs_mask = digitalPinToBitMask(cs);
	SPI.begin();
    SPI.setDataMode(0);
    SPI.setBitOrder(MSBFIRST);
    chipSelect = cs;
    pageOverflow = overflow;
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
bool SPIFlash::_notBusy(uint32_t timeout) {
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
bool SPIFlash::_writeEnable(void) {
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
bool SPIFlash::_writeDisable(void) {
	_cmd(WRITEDISABLE);
	_chipDeselect();
}

//Empties the values stored in any array by setting all the values of all the bits in the array to 1
void SPIFlash::_empty(uint8_t *array) {
	int i;
	for (i = 0; i < arrayLen(array); ++i)
		array[i] = 0xFF;
}


//Gets address from page number and offset. Takes two arguments:
// 1. page_number --> Any page number from 0 to 4095
// 2. offset --> Any offset within the page - from 0 to 255
uint32_t SPIFlash::_getAddress(uint16_t page_number, uint8_t offset) {
	uint32_t address = page_number << 8;
	address += offset;
	return address;
}


// Checks for presence of chip by requesting JEDEC ID
bool SPIFlash::_getJedecId (byte *b1, byte *b2, byte *b3) {
  _cmd(JEDECID);
  *b1 = xfer(0); // manufacturer id
  *b2 = xfer(0); // memory type
  *b3 = xfer(0); // capacity
  _chipDeselect();
  if (!_notBusy())
  	return false;
  return true;
}  

//Checks to see if pageOverflow is permitted and assists with determining next address to write to.
uint8_t SPIFlash::_addressCheck(uint32_t address)
{
  if (address > 0xFFFFF) {
    if (!pageOverflow) {
      return (1);
    }
    else
    return (2);  
  }
  else if (address < 0xFFFFF) {
    return (0);
  } 
}

//Initiates read operation - but data is not read yet
bool SPIFlash::_beginRead(uint32_t address) {
	if((address >= CAPACITY) || !_notBusy())
		return false;

	_cmd(READDATA);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);

	//SPI data lines are left open until _endProcess() is called

	return true;
}

//Reads next byte. Call 'n' times to read 'n' number of bytes. Should be called after _beginRead()
uint8_t SPIFlash::_readNextByte(void) {
	return xfer(0);
}

//Initiates write operation - but data is not written yet
bool SPIFlash::_beginWrite(uint32_t address) {
	if((address >= CAPACITY) || !_notBusy())
		return false;

	_cmd(PAGEPROG);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);

	//SPI data lines are left open until _endProcess() is called

	return true;
}

//Writes next byte. Call 'n' times to read 'n' number of bytes. Should be called after _beginWrite()
bool SPIFlash::_writeNextByte(uint8_t c) {
	xfer(c);
	return  true;
}

//Stops all operations. Should be called after all the required data is read/written from repeated _readNextByte()/_writeNextByte() calls
void SPIFlash::_endProcess(void) {
	_chipDeselect();
	delay(3);
}

//Prints hex/dec formatted data from page reads - for debugging
void SPIFlash::_printPageBytes(uint8_t *data_buffer, uint8_t outputType) {
	char buffer[10];
	for (int a = 0; a < 16; ++a) {
		for (int b = 0; b < 16; ++b) {
			if (outputType == 1) {
				sprintf(buffer, "%02x", data_buffer[a * 16 + b]);
				Serial.print(buffer);
			}
			else if (outputType == 2) {
				uint8_t x = data_buffer[a * 16 + b];
				if (x < 10) Serial.print("0");
				if (x < 100) Serial.print("0");
				Serial.print(x);
				Serial.print(',');
			}
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
      char buffer[64];
      sprintf(buffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh", b1, b2, b3);
      Serial.println(buffer);
}

// Reads a byte of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
uint8_t SPIFlash::readByte(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number >= 4096 || offset >= 256) {
		uint32_t address = _getAddress(page_number, offset);
		_beginRead(address);
		uint8_t var = _readNextByte();
		_endProcess();
		return var;
	}
}

// Reads an array of bytes starting from a specific location in a page. Takes three arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data_buffer --> The array of bytes to be written to from the flash memory - starting at the offset on the page indicated
bool  SPIFlash::readBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer) {
	 if(!_notBusy() || page_number >= 4096 || offset >= 256)
	 	return false;

	 uint32_t address = _getAddress(page_number, offset);
	 uint16_t length = arrayLen(data_buffer);
	 _beginRead(address);
	 for (int a = 0; a < length; ++a) {
	 	data_buffer[a] = _readNextByte();
	 	address++;
	 	uint8_t addressCheck = _addressCheck(address);
		
		switch (addressCheck) {
			case 0:					// Not at end of memory
			break;

			case 1:					// At end of memory - (!pageOverflow)
			Serial.println(F("End of memory bank"));
			while (1);
			break;

			case 2:					// At end of memory - (pageOverflow)
			address = 0x00;
			break;
		}

	 }
	 _endProcess();

	 return true;
}

// Reads a page of data into a page buffer
bool  SPIFlash::readPage(uint16_t page_number, uint8_t *data_buffer) {
	 if(!_notBusy() || page_number >= 4096)
	 	return false;

	 uint8_t offset = 0;
	 uint32_t address = _getAddress(page_number, offset);
	 
	 _beginRead(address);
	 for (int a = 0; a < 256; ++a) {
	 	data_buffer[a] = _readNextByte();
	 }
	 _endProcess();

	 return true;
}

// Writes a byte of data to a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> One byte of data to be written to a particular location on a page
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	uint32_t address = _getAddress(page_number, offset);
	_beginWrite(address);
	_writeNextByte(data);
	_endProcess();

	if (errorCheck)
	{
		_beginRead(address);
		bool result = data == _readNextByte() ? true : false;
		_endProcess();
		return result;
	}
	else
		return true;

}

// Writes an array of bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> An array of bytes to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	uint32_t address = _getAddress(page_number, offset);

	uint16_t n;
	uint16_t maxBytes = 256-(address%256);  // Force the first set of bytes to stay within the first page
	uint16_t data_offset = 0;

	uint16_t length = arrayLen(data_buffer);

	while (length > 0)
	{
		n = (length<=maxBytes) ? length : maxBytes;
		_beginWrite(address);
		for (uint16_t i = 0; i < n; ++i)
			_writeNextByte(data_buffer[data_offset+i]);
		_endProcess();

		address += n;  // Adjust the address and remaining bytes by what we've just transferred.
		uint8_t addressCheck = _addressCheck(address);
		
		switch (addressCheck) {
			case 0:					// Not at end of memory
			break;

			case 1:					// At end of memory - (!pageOverflow)
			Serial.println(F("Out of memory."));
			while (1);
			break;

			case 2:					// At end of memory - (pageOverflow)
			address = 0x00;
			break;
		}
		data_offset += n;
		length -= n;
		maxBytes = 256;   // Now we can do up to 256 bytes per loop
	}

	if (errorCheck)
	{
		address = _getAddress(page_number, offset);
		length = arrayLen(data_buffer);
		_beginRead(address);
		while (length > 0) {
			uint16_t j;
			if (data_buffer[j++] == _readNextByte())
				length--;
			else
				return false;
		}
		_endProcess();
		return true;
	}
	else
		return true;

}

// Writes a page of data from a data_buffer array. Make sure the sizeOf(uint8_t data_buffer[]) == 256. 
//	errorCheck --> Turned on by default. Checks for writing errors.
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096)
		return false;

	char buffer[28];
	uint32_t address = _getAddress(page_number);

	_beginWrite(address);
	for (uint16_t i = 0; i < 256; ++i)
		_writeNextByte(data_buffer[i]);
	_endProcess();
	
	if (errorCheck)
	{
		_beginRead(address);
		uint16_t i;
		for (i = 0; i < 256; ++i)
		{
			if (data_buffer[i] != _readNextByte())
				return false;
		}
		sprintf(buffer, "Writing page (%04x) done", page_number);
		Serial.println(buffer);
		return true;
	}
	else
	{
		sprintf(buffer, "Writing page (%04x) done", page_number);
		Serial.println(buffer);
		return true;
	}
}

//Erases one 4k sector containing the page to be erased. The sectors are numbered 0 - 255 containing 16 pages each.
// Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255
bool SPIFlash::eraseSector(uint16_t page_number) {
	if(!_notBusy() || !_writeEnable())
		return false;

	uint32_t address = _getAddress(page_number);
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

//Erases one 32k block containing the page to be erased. The blocks are numbered 0 - 31 containing 128 pages each.
// Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31
bool SPIFlash::eraseBlock32K(uint16_t page_number) {
	if(!_notBusy() || !_writeEnable())
		return false;

	uint32_t address = _getAddress(page_number);
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

//Erases one 64k block containing the page to be erased. The blocks are numbered 0 - 15 containing 256 pages each.
// Page 0-255 --> Block 0; Page 256-511 --> Block 1;......Page 3840-4095 --> Block 15
bool SPIFlash::eraseBlock64K(uint16_t page_number) {
	if(!_notBusy() || !_writeEnable())
		return false;

	uint32_t address = _getAddress(page_number);
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
bool SPIFlash::eraseChip(void) {
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
bool SPIFlash::suspendProg(void) {
	if(_notBusy())
		return false;

	_cmd(SUSPEND);
	_chipDeselect();

	delay(20);			//Max suspend enable time according to the Datasheet

	if(!_notBusy())
		return false;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {
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
bool SPIFlash::powerDown(void) {
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
bool SPIFlash::powerUp(void) {
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
//         These functions return data to Serial stream. 	          //
//         Declares Serial.begin() if not previously declared.        //
//                 Initiates Serial at 38400 baud.                    //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Reads a page of data and prints it to Serial stream. Make sure the sizeOf(uint8_t data_buffer[]) == 256.
void SPIFlash::printPage(uint16_t page_number, uint8_t outputType) {
	if(!Serial)
		Serial.begin(38400);

	char buffer[24];
	sprintf(buffer, "Reading page (%04x)", page_number);
	Serial.println(buffer);

	uint8_t data_buffer[256];
	readPage(page_number, data_buffer);
	_printPageBytes(data_buffer, outputType);
}

//Reads all pages on Flash chip and dumps it to Serial stream. 
//This function is useful when extracting data from a flash chip onto a computer as a text file.
void SPIFlash::printAllPages(uint8_t outputType) {
	if(!Serial)
		Serial.begin(38400);

	Serial.println("Reading all pages");
	uint8_t data_buffer[256];

	for (int a = 0; a < 4096; ++a) {
		readPage(a, data_buffer);
		_printPageBytes(data_buffer, outputType);
  }
}
