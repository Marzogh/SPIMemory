/* Arduino SPIFlash Library v.1.3.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 30/08/2015
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Uncomment the code below to run a diagnostic if your flash 	  //
//	   						does not respond   						  //
//																	  //
// Error codes will be generated and errors will be printed to Serial //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//#define RUNDIAGNOSTIC												  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define _MANID		 0xEF
#define PAGESIZE	 0x100

#define	MANID		 0x90
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

#define arrayLen(x)  	(sizeof(x) / sizeof(*x))
#define lengthOf(x)  	(sizeof(x))/sizeof(byte)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     					   List of Error codes						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 #ifdef RUNDIAGNOSTIC

 #define SUCCESS 	 	0x00
 #define CALLBEGIN		0x01
 #define UNKNOWNCHIP	0x02
 #define UNKNOWNCAP		0x03
 #define CHIPBUSY		0x04
 #define OUTOFBOUNDS	0x05
 #define CANTENWRITE	0x06
 #define OUTOFMEM		0x07
 #define OUTOFPAGE		0x08

 #endif
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

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
		if((millis()-startTime) > timeout){
			#ifdef RUNDIAGNOSTIC
			errorcode = CHIPBUSY;
			_errorCodeCheck();
			while(1);
			#endif

			return false;
		}
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
	if(state & WRTEN)
		return true;
	else {
		#ifdef RUNDIAGNOSTIC
		errorcode = CANTENWRITE;
 		_errorCodeCheck();
 		while(1);
 		#endif

		return false;
	}
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

#ifdef RUNDIAGNOSTIC
//Checks the errorcode on the current function and issues an error notice
void SPIFlash::_errorCodeCheck(void) {
	switch (errorcode) {
		case SUCCESS:
		Serial.println("Operation successful.");
		break;

		case CALLBEGIN:
		Serial.println ("Please make sure .begin() has been called in setup().");
		break;

		case UNKNOWNCHIP:
		Serial.println("Unknown chip manufacturer.");
		break;

		case UNKNOWNCAP:
		Serial.println("Unknown chip capacity.");
		break;

		case CHIPBUSY:
		Serial.println("Chip busy.");
		break;

		case OUTOFBOUNDS:
		Serial.println("Address out of bounds. Please check if .begin() has been called in setup().");
		break;

		case CANTENWRITE:
		Serial.println("Unable to _writeEnable. Check wiring/chip.");
		break;

		case OUTOFMEM:
		Serial.println("Pagenumber outside maximum");
		break;

		case OUTOFPAGE:
		Serial.println("Offset is outside page");
		break;

		default:
		break;
	}
}
#endif

//Gets address from page number and offset. Takes two arguments:
// 1. page_number --> Any page number from 0 to maxPage
// 2. offset --> Any offset within the page - from 0 to 255
uint32_t SPIFlash::_getAddress(uint16_t page_number, uint8_t offset) {
	uint32_t address = page_number;
	address = (address << 8) + offset;
	if (!_addressCheck(address)){
		#ifdef RUNDIAGNOSTIC
		errorcode = OUTOFBOUNDS;
 		_errorCodeCheck();
 		while(1);
 		#endif
	}
	else
	return address;
}

//Checks the device ID to establish storage parameters
bool SPIFlash::_getManId(uint8_t *b1, uint8_t *b2) {
	if(!_notBusy())
		return false;
	_cmd(MANID);
	xfer(0);
	xfer(0);
	xfer(0);
	*b1 = xfer(0);
	*b2 = xfer(0);
	_chipDeselect();
	return true;
}

//Checks for presence of chip by requesting JEDEC ID
bool SPIFlash::_getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3) {
  if(!_notBusy())
  	return false;
  _cmd(JEDECID);
  *b1 = xfer(0); // manufacturer id
  *b2 = xfer(0); // memory type
  *b3 = xfer(0);// capacity
  _chipDeselect();
  return true;
}  

//Checks to see if pageOverflow is permitted and assists with determining next address to read/write.
//Sets the global address variable
bool SPIFlash::_addressCheck(uint32_t address)
{
  if (address >= capacity) {
    if (!pageOverflow)
    return false;					// At end of memory - (!pageOverflow)

    else {
    address = 0x00;					// At end of memory - (pageOverflow)
    return true;
    }

  }
  else if (address < capacity) {
  	address = address;
    return true;				// Not at end of memory
  } 
}

//Double checks all parameters before calling a Read
uint32_t SPIFlash::_prepRead(uint16_t page_number, uint8_t offset) {
	if(!_notBusy())
		return false;

	uint32_t address = _getAddress(page_number, offset);
			return address;

	return true;
}
//Initiates read operation - but data is not read yet
void SPIFlash::_beginRead(uint32_t address) {

	_cmd(READDATA);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);

	//SPI data lines are left open until _endProcess() is called
}

//Reads next byte. Call 'n' times to read 'n' number of bytes. Should be called after _beginRead()
uint8_t SPIFlash::_readNextByte(void) {
	return xfer(0);
}

//Reads a byte of data from a specific location in a page. Takes one argument -
//  1. address --> address to read from
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
 uint8_t SPIFlash::_readByte(uint32_t address) {
 	uint8_t data;
 	if(!_notBusy()||!_addressCheck(address))
		return false;
 	_beginRead(address);
 	data = _readNextByte();
 	_endProcess();
 	
 	return data;
 
 }

//Double checks all parameters before initiating a write
uint32_t SPIFlash::_prepWrite(uint16_t page_number, uint8_t offset) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	uint32_t address = _getAddress(page_number, offset);
		return address;
}

//Initiates write operation - but data is not written yet
bool SPIFlash::_beginWrite(uint32_t address) {

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

// Writes a byte of data to a specific location in a page. Takes three arguments -
//  1. address --> address to write to
//  2. data --> One byte of data to be written to a particular location on a page
//  3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::_writeByte(uint32_t address, uint8_t data, bool errorCheck) {
 	
 	if(!_notBusy()||!_writeEnable()||!_addressCheck(address))
 		return false;
 
 	_beginWrite(address);
 	_writeNextByte(data);
 	_endProcess();
 
 	if (errorCheck)
 	{
 		_beginRead(address);
 		uint8_t result = _readNextByte();
 		_endProcess();
 		
 		/*Serial.print("Data: ");
 		Serial.println(data);
 		Serial.print("Result: ");
 		Serial.println(result);*/

 		return data == result ? true : false;
 	}
 	else
 		return true;
 
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

//Identifies chip and establishes parameters
void SPIFlash::begin() {
	//Get Manfucturer/Device ID so the library can identify the chip
    uint8_t manID, devID ;
    _getManId(&manID, &devID);

    if (manID != _MANID){		//If the chip is not a Winbond Chip
    	while(1);
    	#ifdef RUNDIAGNOSTIC
    	errorcode = UNKNOWNCHIP;		//Error code for unidentified chip
    	_errorCodeCheck();
    	#endif
    }

    //Check flash memory type and identify capacity
    uint8_t i;
    //capacity;
    for (i = 0; i < sizeof(devType); i++)
    {
    	if (devID == devType[i])
    		capacity = memSize[i];
    }
    if (capacity == 0) {
    	while(1);
    	#ifdef RUNDIAGNOSTIC
    	errorcode = UNKNOWNCAP;		//Error code for unidentified capacity
    	_errorCodeCheck();
    	#endif
    }

   	maxPage = capacity/PAGESIZE;

   	#ifdef RUNDIAGNOSTIC
    char buffer[64];
    sprintf(buffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %lu\nmaxPage: %d", manID, devID, capacity, maxPage);
    Serial.println(buffer);
    errorcode = SUCCESS;			//Successful chip detect
    _errorCodeCheck();
    #endif

}

//Checks for and initiates the chip by requesting JEDEC ID which is returned as a 32 bit int
uint16_t SPIFlash::getManID() {
	uint8_t b1, b2;
    _getManId(&b1, &b2);
    uint32_t id = b1;
    id = (id << 8)|(b2 << 0);
    return id;
}

//Checks for and initiates the chip by requesting JEDEC ID which is returned as a 32 bit int
uint32_t SPIFlash::getJEDECID() {
	uint8_t b1, b2, b3;
    _getJedecId(&b1, &b2, &b3);
    uint32_t id = b1;
    id = (id << 8)|(b2 << 0);
    id = (id << 8)|(b3 << 0);
    return id;
}

// Reads a byte of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
uint8_t SPIFlash::readByte(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);

	uint8_t data;
	_beginRead(address);
	data = _readNextByte();
	_endProcess();
	return data;
}

// Reads a char of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
int8_t SPIFlash::readChar(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);
	int8_t data;
	_beginRead(address);
	data = _readNextByte();
	_endProcess();
	return data;
}

// Reads an array of bytes starting from a specific location in a page. Takes three arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data_buffer --> The array of bytes to be read from the flash memory - starting at the offset on the page indicated
void  SPIFlash::readBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer) {
	uint32_t address = _prepRead(page_number, offset);
	uint16_t length = arrayLen(data_buffer);
	_beginRead(address);
	for (int a = 0; a < length; a++) {
		data_buffer[a] = _readNextByte();
		_addressCheck(address++);
	}
	_endProcess();
}

// Reads an unsigned int of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
uint16_t SPIFlash::readWord(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);
	union
	{
		byte b[sizeof(uint16_t)];
		uint16_t I;
	} data;
	_beginRead(address);
	for (int i=0; i < (sizeof(int16_t)); i++) {
	data.b[i] = _readNextByte();
	}
	_endProcess();
	return data.I;
}

// Reads a signed int of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
int16_t SPIFlash::readShort(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);
	union
	{
		byte b[sizeof(int16_t)];
		int16_t s;
	} data;
	_beginRead(address);
	for (int i=0; i < (sizeof(int16_t)); i++) {
		data.b[i] = _readNextByte();
	}
	_endProcess();
	return data.s;
}

// Reads an unsigned long of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
uint32_t SPIFlash::readULong(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);
	union
	{
		byte b[sizeof(uint32_t)];
		uint32_t l;
	} data;
	_beginRead(address);
	for (int i=0; i < (sizeof(uint32_t)); i++) {
		data.b[i] = _readNextByte();
	}
	_endProcess();
	return data.l;
}

// Reads a signed long of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
int32_t SPIFlash::readLong(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);
	union
	{
		byte b[sizeof(int32_t)];
		int32_t l;
	} data;
	_beginRead(address);
	for (int i=0; i < (sizeof(int32_t)); i++) {
		data.b[i] = _readNextByte();
	}
	_endProcess();
	return data.l;
}

// Reads a signed long of data from a specific location in a page. Takes two arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
float SPIFlash::readFloat(uint16_t page_number, uint8_t offset) {
	uint32_t address = _prepRead(page_number, offset);
	union
	{
		byte b[(sizeof(float))];
		float f;
	} data;
	_beginRead(address);
	for (int i=0; i < (sizeof(float)); i++) {
		data.b[i] = _readNextByte();
	}
	_endProcess();
	return data.f;
}

// Reads a page of data into a page buffer
void  SPIFlash::readPage(uint16_t page_number, uint8_t *data_buffer) {
	uint32_t address = _prepRead(page_number);
	_beginRead(address);
	for (int a = 0; a < 256; ++a) {
		data_buffer[a] = _readNextByte();
	}
	_endProcess();
}

// Writes a byte of data to a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> One byte of data to be written to a particular location on a page
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);
	
	_beginWrite(address);
	_writeNextByte(data);
	_endProcess();

	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);

}

// Writes a byte of data to a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> One byte of data to be written to a particular location on a page
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);
	
	_beginWrite(address);
	_writeNextByte(data);
	_endProcess();

	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);

}

// Writes an array of bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> An array of bytes to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeBytes(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);

	uint16_t n;
	uint16_t maxBytes = PAGESIZE-(address % PAGESIZE);  // Force the first set of bytes to stay within the first page
	uint16_t data_offset = 0;

	uint16_t length = arrayLen(data_buffer);
	_beginWrite(address);

	while (length > 0)
		{
		n = (length<=maxBytes) ? length : maxBytes;
		for (uint16_t i = 0; i < n; ++i)
			_writeNextByte(data_buffer[data_offset+i]);
		if (!_addressCheck(address+=n))
			return false;  // Adjust the address and remaining bytes by what we've just transferred.
		data_offset += n;
		length -= n;
		maxBytes = 256;   // Now we can do up to 256 bytes per loop
	}
	return true;
}

// Writes an unsigned int as two bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> An int to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);

	union
	{
		uint8_t b[sizeof(data)];
		uint16_t w;
	} var;
	var.w = data;
	_beginWrite(address);
	for (int j = 0; j < (sizeof(data)); j++) {
		_writeNextByte(var.b[j]);
	}
	_endProcess();

	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}

// Writes a signed int as two bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> An int to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);

	union
	{
		uint8_t b[sizeof(data)];
		int16_t s;
	} var;
	var.s = data;
	_beginWrite(address);
	for (int j = 0; j < (sizeof(data)); j++) {
		_writeNextByte(var.b[j]);
	}
	_endProcess();

	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}

// Writes an unsigned long as four bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> A long to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);

	union
	{
		uint8_t b[sizeof(data)];
		uint32_t l;
	} var;
	var.l = data;
	_beginWrite(address);
	for (int j = 0; j < (sizeof(data)); j++) {
		_writeNextByte(var.b[j]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}

// Writes a signed long as four bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> A long to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);

	union
	{
		uint8_t b[sizeof(data)];
		int32_t l;
	} var;
	var.l = data;
	_beginWrite(address);
	for (int j = 0; j < (sizeof(data)); j++) {
		_writeNextByte(var.b[j]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}

// Writes a float as four bytes starting from a specific location in a page. Takes four arguments -
//  1. page --> Any page number from 0 to maxPage
//  2. offset --> Any offset within the page - from 0 to 255
//  3. data --> A long to be written to the flash memory - starting at the offset on the page indicated
//	4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);

	union
	{
		uint8_t b[sizeof(data)];
		float f;
	} var;
	var.f = data;
	_beginWrite(address);
	for (int j = 0; j < (sizeof(data)); j++) {
		_writeNextByte(var.b[j]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}

// Writes a page of data from a data_buffer array. Make sure the sizeOf(uint8_t data_buffer[]) == 256. 
//	errorCheck --> Turned on by default. Checks for writing errors.
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck) {
	uint32_t address = _prepWrite(page_number);

	_beginWrite(address);
	for (uint16_t i = 0; i < PAGESIZE; ++i)
		_writeNextByte(data_buffer[i]);
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data_buffer);

	#ifdef RUNDIAGNOSTIC
	char buffer[64];
	sprintf(buffer, "Writing page (%04x) done", page_number);
	Serial.println(buffer);
	#endif
}


//Erases one 4k sector containing the page to be erased. The sectors are numbered 0 - 255 containing 16 pages each.
// Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255
bool SPIFlash::eraseSector(uint16_t page_number) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	uint32_t address = _getAddress(page_number);

	_cmd(SECTORERASE);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	_chipDeselect();

	if(!_notBusy(410L))
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
	if(!_notBusy()||!_writeEnable())
 		return false;

	uint32_t address = _getAddress(page_number);

	_cmd(BLOCK32ERASE);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	_chipDeselect();

	if(!_notBusy(410L))
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
	if(!_notBusy()||!_writeEnable())
 		return false;

	uint32_t address = _getAddress(page_number);

	_cmd(BLOCK64ERASE);
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	_chipDeselect();

	if(!_notBusy(410L))
		return false;	//Datasheet says erasing a sector takes 400ms max

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Erases whole chip. Think twice before using.
bool SPIFlash::eraseChip(void) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	_cmd(CHIPERASE);
	_chipDeselect();

	if(!_notBusy(7000L))
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
	if(!_notBusy())
		return false;

	_cmd(SUSPEND);
	_chipDeselect();

	if(!_notBusy(20L))	//Max suspend Enable time according to datasheet
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
