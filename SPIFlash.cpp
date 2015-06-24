/* Arduino SPIFlash Library v.1.3.0
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
#define lengthOf(x)  (sizeof(x))/sizeof(byte)

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
bool SPIFlash::_getAddress(uint16_t page_number, uint8_t offset) {
	/*uint32_t address = page_number << 8;
	address += offset;*/
	currentAddress = (page_number >>  8) + (page_number >>  0) + offset;
	if (!_addressCheck(currentAddress))
		return false;
	else
	return true;
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

//Checks to see if pageOverflow is permitted and assists with determining next address to read/write.
//Sets the global currentAddress variable
bool SPIFlash::_addressCheck(uint32_t address)
{
  if (address > 0xFFFFF) {
    if (!pageOverflow) {
      Serial.println(F("End of memory bank"));
	  return false;					// At end of memory - (!pageOverflow)
    }
    else {
    currentAddress = 0x00;					// At end of memory - (pageOverflow)
    return true;
    }

  }
  else if (address < 0xFFFFF) {
  	currentAddress = address;
    return true;				// Not at end of memory
  } 
}

uint8_t SPIFlash::_readByte(uint32_t address) {
		_beginRead(address);
		uint8_t data = _readNextByte();
		_endProcess();
		return data;
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

// Writes a byte of data to a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> One byte of data to be written to a particular location on a page
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::_write8Bit(uint32_t address, uint8_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || address >= CAPACITY)
		return false;

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

// Writes two bytes of data to a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> Two bytes of data to be written to a particular location on a page
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::_write16Bit(uint32_t address, uint16_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || address >= CAPACITY)
		return false;
	union
	{
		byte b[2];
		uint16_t i;
	} var;
	var.i = data;
	//_beginRead(address);
	for (int j = 0; j < 2; j++) {
		_write8Bit(address+j, var.b[j], errorCheck);
	}
	
}

bool SPIFlash::_write32Bit(uint32_t address, uint32_t data, bool errorCheck) {
	union
	{
		byte b[4];
		float f;
	} var;
	var.f = data;
	for (int i = 0; i < 4; i++) {
		_write8Bit(address+i, var.b[i], errorCheck);
	}
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
	if(!_notBusy() || page_number >= 4096 || offset >= 256) 
		return false;
	else
	{
		if (!_getAddress(page_number, offset))
			return false;
		else {
			uint8_t data;
			data = _readByte(currentAddress);
			return data;
		}
	}
}

// Reads a char of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
int8_t SPIFlash::readChar(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number >= 4096 || offset >= 256) 
		return false;
	else
	{
		if (!_getAddress(page_number, offset))
			return false;
		else {
			int8_t data;
			data = _readByte(currentAddress);
			return data;
		}
	}
}

// Reads an array of bytes starting from a specific location in a page. Takes three arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data_buffer --> The array of bytes to be read from the flash memory - starting at the offset on the page indicated
bool  SPIFlash::readBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer) {
	 if(!_notBusy() || page_number >= 4096 || offset >= 256)
	 	return false;

	 if (!_getAddress(page_number, offset))
			return false;
	else {
	 	uint16_t length = arrayLen(data_buffer);
	 	_beginRead(currentAddress);
	 	for (int a = 0; a < length; ++a) {
	 		data_buffer[a] = _readNextByte();
	 		_addressCheck(currentAddress++);
	 	}
	 	_endProcess();
	 }
	return true;
}

// Reads an unsigned int of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
uint16_t SPIFlash::readWord(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
			return false;
	else {
		union
		{
			byte b[2];
			uint16_t I;
		} data;
		for (int i=0; i < 2; i++) {
		data.b[i] = _readByte(currentAddress +i);
		}
		return data.I;
	}
}

// Reads a signed int of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
int16_t SPIFlash::readShort(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number >= 4096 || offset >= 256)
		return false;
	if (!_getAddress(page_number, offset))
			return false;
	else {
		union
		{
			byte b[2];
			int16_t s;
		} data;
		for (int i=0; i < 2; i++) {
			data.b[i] = _readByte(currentAddress +i);
		}
		return data.s;
	}
}

// Reads an unsigned long of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
uint32_t SPIFlash::readULong(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number >= 4096 || offset >= 256)
		return false;
	if (!_getAddress(page_number, offset))
			return false;
	else {
		union
		{
			byte b[4];
			uint32_t l;
		} data;
		for (int i=0; i < 4; i++) {
			data.b[i] = _readByte(currentAddress +i);
		}
		return data.l;
	}
}

// Reads a signed long of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
int32_t SPIFlash::readLong(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number > 4096 || offset > 256) 
		return false;
	if (!_getAddress(page_number, offset))
			return false;
	else {
		union
		{
			byte b[4];
			int32_t l;
		} data;
		for (int i=0; i < 4; i++) {
			data.b[i] = _readByte(currentAddress +i);
		}
		return data.l;
	}
}

// Reads a signed long of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
float SPIFlash::readFloat(uint16_t page_number, uint8_t offset) {
	if(!_notBusy() || page_number > 4096 || offset > 256)
		return false;
	if (!_getAddress(page_number, offset))
			return false;
	else {
		union
		{
			byte b[4];
			float f;
		} data;
		for (int i=0; i < 4; i++) {
			data.b[i] = _readByte(currentAddress +i);
		}
		return data.f;
	}
}

// Reads a page of data into a page buffer
bool  SPIFlash::readPage(uint16_t page_number, uint8_t *data_buffer) {
	 if(!_notBusy() || page_number >= 4096)
	 	return false;
	 if (!_getAddress(page_number))
			return false;
	else {
		_beginRead(currentAddress);
		for (int a = 0; a < 256; ++a) {
	 		data_buffer[a] = _readNextByte();
	 	}
	 	_endProcess();
	 	return true;
	}
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

	if (!_getAddress(page_number, offset))
		return false;
	else
	_write8Bit(currentAddress, data, errorCheck);

}

// Writes a byte of data to a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> One byte of data to be written to a particular location on a page
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
		return false;
	else 
	_write8Bit(currentAddress, data, errorCheck);

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

	if (!_getAddress(page_number, offset))
		return false;
	else {
		uint16_t n;
		uint16_t maxBytes = 256-(currentAddress%256);  // Force the first set of bytes to stay within the first page
		uint16_t data_offset = 0;

		uint16_t length = arrayLen(data_buffer);

		while (length > 0)
		{
			n = (length<=maxBytes) ? length : maxBytes;
			for (uint16_t i = 0; i < n; ++i)
				_write8Bit(currentAddress, data_buffer[data_offset+i]);
			if (!_addressCheck(currentAddress+=n))
				return false;  // Adjust the address and remaining bytes by what we've just transferred.
			data_offset += n;
			length -= n;
			maxBytes = 256;   // Now we can do up to 256 bytes per loop
		}
		return true;
	}
}

// Writes an unsigned int as two bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> An int to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
			return false;
	else {
		if (!_write16Bit(currentAddress, data))
     		return false;
     	else
     		return true;
	}
}

// Writes a signed int as two bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> An int to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
			return false;
	else {
		//One = Most significant -> Two = Least significant byte
		uint8_t two = (data & 0xFF);
     	uint8_t one = ((data >> 8) & 0xFF);
    	if (_write8Bit(currentAddress, two, errorCheck) && _write8Bit((currentAddress+=1), one, errorCheck))
     		return true;
	}
}

// Writes an unsigned long as four bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> A long to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
			return false;
	else {
		//One = Most significant -> Two = Least significant byte
	    uint8_t four = (data & 0xFF);
	    uint8_t three = ((data >> 8) & 0xFF);
	    uint8_t two = ((data >> 16) & 0xFF);
	    uint8_t one = ((data >> 24) & 0xFF);
	    if (_write8Bit(currentAddress, four, errorCheck) && _write8Bit((currentAddress+=1), three, errorCheck) 
	    	&& _write8Bit((currentAddress+=1), two, errorCheck) && _write8Bit((currentAddress+=1), one, errorCheck))
	    	return true;
	}
}

// Writes a signed long as four bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> A long to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
			return false;
	else {
		//One = Most significant -> Two = Least significant byte
	    uint8_t four = (data & 0xFF);
	    uint8_t three = ((data >> 8) & 0xFF);
	    uint8_t two = ((data >> 16) & 0xFF);
	    uint8_t one = ((data >> 24) & 0xFF);
	    if (_write8Bit(currentAddress, four, errorCheck) && _write8Bit((currentAddress+=1), three, errorCheck)
	    	&& _write8Bit((currentAddress+=1), two, errorCheck) && _write8Bit((currentAddress+=1), one, errorCheck))
	    	return true;
	}
}

// Writes a float as four bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to 4095
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> A long to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096 || offset >= 256)
		return false;

	if (!_getAddress(page_number, offset))
			return false;
	else
	_write32Bit(currentAddress, data);
}

// Writes a page of data from a data_buffer array. Make sure the sizeOf(uint8_t data_buffer[]) == 256. 
//	errorCheck --> Turned on by default. Checks for writing errors.
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck) {
	if(!_notBusy() || !_writeEnable() || page_number >= 4096)
		return false;

	char buffer[28];
	if (!_getAddress(page_number))
			return false;
	else {
		for (uint16_t i = 0; i < 256; ++i) {
			if(!_write8Bit(currentAddress, data_buffer[i], errorCheck))
				return false;
		}
		return true;
	}
}

//Erases one 4k sector containing the page to be erased. The sectors are numbered 0 - 255 containing 16 pages each.
// Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255
bool SPIFlash::eraseSector(uint16_t page_number) {
	if(!_notBusy() || !_writeEnable())
		return false;

	if (!_getAddress(page_number))
			return false;
	else {
		_cmd(SECTORERASE);
		(void)xfer(currentAddress >> 16);
		(void)xfer(currentAddress >> 8);
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
}

//Erases one 32k block containing the page to be erased. The blocks are numbered 0 - 31 containing 128 pages each.
// Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31
bool SPIFlash::eraseBlock32K(uint16_t page_number) {
	if(!_notBusy() || !_writeEnable())
		return false;

	if (!_getAddress(page_number))
			return false;
	else {
		_cmd(BLOCK32ERASE);
		(void)xfer(currentAddress >> 16);
		(void)xfer(currentAddress >> 8);
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
}

//Erases one 64k block containing the page to be erased. The blocks are numbered 0 - 15 containing 256 pages each.
// Page 0-255 --> Block 0; Page 256-511 --> Block 1;......Page 3840-4095 --> Block 15
bool SPIFlash::eraseBlock64K(uint16_t page_number) {
	if(!_notBusy() || !_writeEnable())
		return false;

	if (!_getAddress(page_number))
			return false;
	else {
		_cmd(BLOCK64ERASE);
		(void)xfer(currentAddress >> 16);
		(void)xfer(currentAddress >> 8);
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
//                 Initiates Serial at 115200 baud.                    //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Reads a page of data and prints it to Serial stream. Make sure the sizeOf(uint8_t data_buffer[]) == 256.
void SPIFlash::printPage(uint16_t page_number, uint8_t outputType) {
	if(!Serial)
		Serial.begin(115200);

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
		Serial.begin(115200);

	Serial.println("Reading all pages");
	uint8_t data_buffer[256];

	for (int a = 0; a < 4096; ++a) {
		readPage(a, data_buffer);
		_printPageBytes(data_buffer, outputType);
  }
}
