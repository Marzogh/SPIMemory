/* Arduino SPIFlash Library v.2.2.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 24/11/2015
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
 
#include "SPIFlash.h"

#if defined (__arm__) && defined (__SAM3X8E__)
 #define _delay_us(us) delayMicroseconds(us)
#else
 #include <util/delay.h>
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Uncomment the code below to run a diagnostic if your flash 	  //
//	   						does not respond   						  //
//																	  //
// 		Error codes will be generated and returned on functions		  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//#define RUNDIAGNOSTIC												  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//   Uncomment the code below to increase the speed of the library    //
//	   				by disabling _notPrevWritten()   				  //
//																	  //
// Make sure the sectors being written to have been erased beforehand //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#if defined (__SAM3X8E__)
//#define HIGHSPEED		
#elif defined (__AVR__)
//#define HIGHSPEED													  
#endif																  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define _MANID		 0xEF
#define PAGESIZE	 0x100

#define	MANID		 0x90
#define PAGEPROG     0x02
#define READDATA     0x03
#define FASTREAD	 0x0B
#define WRITEDISABLE 0x04
#define READSTAT1    0x05
#define READSTAT2	 0x35
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
#define SUS 		 0x40
#define DUMMYBYTE	 0xEE

#define arrayLen(x)  	(sizeof(x) / sizeof(*x))
#define lengthOf(x)  	(sizeof(x))/sizeof(byte)
#define maxAddress		capacity
#define NO_CONTINUE		0x00
#define PASS			0x01
#define FAIL			0x00

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
 #define PREVWRITTEN 	0x07
 #define UNKNOWNERROR	0xFF

 #endif
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#if defined (__arm__) && defined (__SAM3X8E__)
 #include <SPI.h>
#elif defined (__AVR__)
	#ifdef __AVR_ATtiny85__
		#define CHIP_SELECT   PORTB &= ~cs_mask;
		#define CHIP_DESELECT PORTB |=  cs_mask;
		#define SPIBIT                      \
		  USICR = ((1<<USIWM0)|(1<<USITC)); \
		  USICR = ((1<<USIWM0)|(1<<USITC)|(1<<USICLK));
		static uint8_t xfer(uint8_t n) {
			USIDR = n;
			SPIBIT
			SPIBIT
			SPIBIT
			SPIBIT
			SPIBIT
			SPIBIT
			SPIBIT
			SPIBIT
			return USIDR;
		}
	#else
		#include <SPI.h>
		#define CHIP_SELECT   *cs_port &= ~cs_mask;
		#define CHIP_DESELECT *cs_port |=  cs_mask;
		#define xfer(n)   SPI.transfer(n)
	#endif
#endif

// Constructor
#if defined (__arm__) && defined (__SAM3X8E__)
SPIFlash::SPIFlash(uint8_t cs, bool overflow) {
	csPin = cs;
	pageOverflow = overflow;
}
#elif defined (__AVR__)
SPIFlash::SPIFlash(uint8_t cs, bool overflow) {
	csPin = cs;
#ifndef __AVR_ATtiny85__
	cs_port = portOutputRegister(digitalPinToPort(csPin));
#endif
	cs_mask = digitalPinToBitMask(csPin);
	SPI.begin();
    SPI.setDataMode(0);
    SPI.setBitOrder(MSBFIRST);
    //SPI.setClockDivider(SPI_CLOCK_DIV2);
    pageOverflow = overflow;
    pinMode(cs, OUTPUT);
}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Private functions used by read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// Select chip and issue command - data to follow
void SPIFlash::_cmd(uint8_t cmd, bool _continue, uint8_t cs) {
	cs = csPin;
 #if defined (__arm__) && defined (__SAM3X8E__)
	if (!_continue)
		SPI.transfer(cs, cmd);
	else
		SPI.transfer(cs, cmd, SPI_CONTINUE);
 #else
	CHIP_SELECT
	(void)xfer(cmd);
 #endif
}

// Checks if status register 1 can be accessed - used during powerdown and power up and for debugging
uint8_t SPIFlash::_readStat1(void) {
	uint8_t stat1;
	_cmd(READSTAT1);
	#if defined (__SAM3X8E__)
	stat1 = SPI.transfer(csPin, 0x00);
	#else
	stat1 = xfer(0);
	CHIP_DESELECT
	#endif
	return stat1;
}

// Checks the erase/program suspend flag before enabling/disabling a program/erase suspend operation
bool SPIFlash::_noSuspend(void) {
	uint8_t state;

	_cmd(READSTAT2);
	#if defined (__arm__) && defined (__SAM3X8E__)
		state = SPI.transfer(csPin, 0x00);
	#else
		state = xfer(0);
		CHIP_DESELECT
	#endif

	if(state & SUS)
		return false;
	return true;
}

// Polls the status register 1 until busy flag is cleared or timeout
bool SPIFlash::_notBusy(uint32_t timeout) {
	uint8_t state;
	uint32_t startTime = millis();

	do {
		_cmd(READSTAT1);
		#if defined (__arm__) && defined (__SAM3X8E__)
		state = SPI.transfer(csPin, 0x00);
	#else
		state = xfer(0);
		CHIP_DESELECT
	#endif
		if((millis()-startTime) > timeout){
			#ifdef RUNDIAGNOSTIC
			errorcode = CHIPBUSY;
			_troubleshoot(errorcode);
			#endif
			return false;
		}
	} while(state & BUSY);
	return true;
}

//Enables writing to chip by setting the WRITEENABLE bit
bool SPIFlash::_writeEnable(void) {
	uint8_t state;

	#if defined (__arm__) && (__SAM3X8E__)
	SPI.transfer(csPin, WRITEENABLE);
	#else
	_cmd(WRITEENABLE);
	CHIP_DESELECT
	#endif

	//verifies that WRITE is enabled
	#if defined (__arm__) && (__SAM3X8E__)
	SPI.transfer(csPin, READSTAT1, SPI_CONTINUE);
	state = SPI.transfer(csPin, 0x00);
	#else
	_cmd(READSTAT1);
	state = xfer(0);
	CHIP_DESELECT
	#endif
	if(state & WRTEN)
		return true;
	else {
		#ifdef RUNDIAGNOSTIC
		errorcode = CANTENWRITE;
 		_troubleshoot(errorcode);
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
	#if defined (__AVR__)
	CHIP_DESELECT
	#endif

	return true;
}

//Gets address from page number and offset. Takes two arguments:
// 1. page_number --> Any page number from 0 to maxPage
// 2. offset --> Any offset within the page - from 0 to 255
uint32_t SPIFlash::_getAddress(uint16_t page_number, uint8_t offset) {
	uint32_t address = page_number;
	address = (address << 8) + offset;

	if (!_addressCheck(address)){
		#ifdef RUNDIAGNOSTIC
		errorcode = OUTOFBOUNDS;
 		_troubleshoot(errorcode);
 		#endif
 		return false;
	}
	else
	return address;
}

//Checks the device ID to establish storage parameters
bool SPIFlash::_getManId(uint8_t *b1, uint8_t *b2) {
	if(!_notBusy())
		return false;
	_cmd(MANID);
	#if defined (__arm__) && defined (__SAM3X8E__)
		SPI.transfer(csPin, 0x00, SPI_CONTINUE);
		SPI.transfer(csPin, 0x00, SPI_CONTINUE);
		SPI.transfer(csPin, 0x00, SPI_CONTINUE);
		*b1 = SPI.transfer(csPin, 0x00, SPI_CONTINUE);
		*b2 = SPI.transfer(csPin, 0x00);
	#else
		xfer(0);
		xfer(0);
		xfer(0);
		*b1 = xfer(0);
		*b2 = xfer(0);
		CHIP_DESELECT
	#endif
	return true;
}

//Checks for presence of chip by requesting JEDEC ID
bool SPIFlash::_getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3) {
  if(!_notBusy())
  	return false;
  _cmd(JEDECID);
  #if defined (__arm__) && defined (__SAM3X8E__)
		*b1 = SPI.transfer(csPin, 0x00, SPI_CONTINUE);		// manufacturer id
		*b2 = SPI.transfer(csPin, 0x00, SPI_CONTINUE);		// manufacturer id
		*b3 = SPI.transfer(csPin, 0x00);					// capacity
  #else
		*b1 = xfer(0); 		// manufacturer id
		*b2 = xfer(0); 		// memory type
		*b3 = xfer(0);		// capacity
		CHIP_DESELECT
  #endif
  return true;
}  

//Identifies the chip
bool SPIFlash::_chipID(void) {
	//Get Manfucturer/Device ID so the library can identify the chip
    uint8_t manID, devID ;
    _getManId(&manID, &devID);

    if (manID != _MANID){		//If the chip is not a Winbond Chip
    	#ifdef RUNDIAGNOSTIC
    	errorcode = UNKNOWNCHIP;		//Error code for unidentified chip
    	_troubleshoot(errorcode);
    	#endif
    	while(1);
    }

    //Check flash memory type and identify capacity
    uint8_t i;
    //capacity & chip name
    for (i = 0; i < sizeof(devType); i++)
    {
    	if (devID == devType[i]) {
    		capacity = memSize[i];
    		name = chipName[i];
    	}
    }
    if (capacity == 0) {
    	#ifdef RUNDIAGNOSTIC
    	errorcode = UNKNOWNCAP;		//Error code for unidentified capacity
    	_troubleshoot(errorcode);
    	#endif
    	while(1);
    }

   	maxPage = capacity/PAGESIZE;

   	/*#ifdef RUNDIAGNOSTIC
    char buffer[64];
    sprintf(buffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %lu\nmaxPage: %d", manID, devID, capacity, maxPage);
    Serial.println(buffer);
    #endif*/
    return true;
}

//Checks to see if pageOverflow is permitted and assists with determining next address to read/write.
//Sets the global address variable
bool SPIFlash::_addressCheck(uint32_t address) {
	#ifdef RUNDIAGNOSTIC
	if (capacity == 0) {
		errorcode = CALLBEGIN;
		_troubleshoot(errorcode);
	}
	#endif
  	if (address >= capacity) {
    	if (!pageOverflow) {
	    	#ifdef RUNDIAGNOSTIC
	    	errorcode = OUTOFBOUNDS;
	    	_troubleshoot(errorcode);
	    	#endif
	    	return false;					// At end of memory - (!pageOverflow)
		}
    else {
    address = 0x00;					// At end of memory - (pageOverflow)
    return true;
    }
  }
  return true;				// Not at end of memory if (address < capacity)
}

//Double checks all parameters before calling a Read
//Has two variants:
//	A. Takes address and returns the address if true, else returns false. Throws an error if there is a problem.
//	B. Takes page_number and offset and returns address. Throws an error if there is a problem
// Variant A
uint32_t SPIFlash::_prepRead(uint32_t address) {
	if(!_notBusy())
		return false;
	if (!_addressCheck(address))
 		return false;
	else
		return address;
}
// Variant B
uint32_t SPIFlash::_prepRead(uint16_t page_number, uint8_t offset) {
	uint32_t address = _getAddress(page_number, offset);
	return _prepRead(address);
}

//Initiates read operation - but data is not read yet
void SPIFlash::_beginRead(uint32_t address) {
	_cmd(READDATA);
	#if defined (__arm__) && defined (__SAM3X8E__)
	SPI.transfer(csPin, address >> 16, SPI_CONTINUE);
  	SPI.transfer(csPin, address >> 8, SPI_CONTINUE);
  	SPI.transfer(csPin, address, SPI_CONTINUE);
  	#else
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);
	#endif
	//SPI data lines are left open until _endProcess() is called
}

//Initiates fast read operation (can operate at the highest possible freq of 104 MHz (at 3.0-3.6V) of 80 MHz (at 2.5-3.6V))
//- but data is not read yet
void SPIFlash::_beginFastRead(uint32_t address) {
	_cmd(FASTREAD);
	#if defined (__arm__) && defined (__SAM3X8E__)
	SPI.transfer(csPin, address >> 16, SPI_CONTINUE);
  	SPI.transfer(csPin, address >> 8, SPI_CONTINUE);
  	SPI.transfer(csPin, address, SPI_CONTINUE);
  	#else
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);
	#endif
	//SPI data lines are left open until _endProcess() is called
}

//Reads next byte. Call 'n' times to read 'n' number of bytes. Should be called after _beginRead()
uint8_t SPIFlash::_readNextByte(bool _continue) {
	uint8_t result;
	#if defined (__arm__) && defined (__SAM3X8E__)
		if (!_continue)
			result = SPI.transfer(csPin, 0x00);
		else
			result = SPI.transfer(csPin, 0x00, SPI_CONTINUE);
	#else
		result = xfer(0x00);
	#endif
	return result;
}

//Double checks all parameters before initiating a write
//Has two variants:
//	A. Takes address and returns address if true or 'false' if false. Throws an error if there is a problem.
//	B. Takes page_number and offset and returns address. Throws an error if there is a problem
// Variant A
uint32_t SPIFlash::_prepWrite(uint32_t address) {
	if(!_notBusy()||!_writeEnable())
 		return false;
	if (!_addressCheck(address)) {
		#ifdef RUNDIAGNOSTIC
		errorcode = OUTOFBOUNDS;
 		_troubleshoot(errorcode);
 		#endif
 		return false;
	}
	else
		return address;
}
// Variant B
uint32_t SPIFlash::_prepWrite(uint16_t page_number, uint8_t offset) {
	uint32_t address = _getAddress(page_number, offset);
	return _prepWrite(address);
}

//Initiates write operation - but data is not written yet
bool SPIFlash::_beginWrite(uint32_t address) {
	_cmd(PAGEPROG);
	#if defined (__arm__) && defined (__SAM3X8E__)
	SPI.transfer(csPin, address >> 16, SPI_CONTINUE);
  	SPI.transfer(csPin, address >> 8, SPI_CONTINUE);
  	SPI.transfer(csPin, address, SPI_CONTINUE);
  	#else
  	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(address);
	#endif
	//SPI data lines are left open until _endProcess() is called
	return true;
}

//Writes next byte. Call 'n' times to read 'n' number of bytes. Should be called after _beginWrite()
bool SPIFlash::_writeNextByte(uint8_t b, bool _continue) {
	#if defined (__arm__) && defined (__SAM3X8E__)
		if (!_continue)
			SPI.transfer(csPin, b);
		else
			SPI.transfer(csPin, b, SPI_CONTINUE);
	#else
		xfer(b);
	#endif
	return  true;
}

//Stops all operations. Should be called after all the required data is read/written from repeated _readNextByte()/_writeNextByte() calls
void SPIFlash::_endProcess(void) {
	#if defined (__arm__) && defined (__SAM3X8E__)
	_delay_us(3);
	#else
	CHIP_DESELECT
	#endif
	_delay_us(3);
}

#ifndef HIGHSPEED
bool SPIFlash::_notPrevWritten(uint32_t address, uint8_t size) {
	uint32_t _size = size;

	/*uint8_t databyte1, databyte2, databyte3;
	databyte1 = readByte(address, true);
	databyte2 = readByte((address + (size/2)), true);
	databyte3 = readByte((address+size), true);
	if (databyte1 != 255 || databyte2 != 255 || databyte3 != 255) {
		#ifdef RUNDIAGNOSTIC
		errorcode = PREVWRITTEN;
 		_troubleshoot(errorcode);
 		#endif
		return false;
	}
	_delay_us(3);
	return true;*/
	uint32_t sampleSize;
	if (_size <= 10) {
		sampleSize = _size;
	}

	if (_size > 10) {
		do {
			sampleSize++;
			_size/=10;
		} while((_size/10) >= 1);
	}

	uint32_t addresses[sampleSize];

	for (uint16_t i = 0; i < sampleSize; i++) {
		addresses[i] = (rand() % size) + address;
	}

	for (uint16_t i = 0; i < sampleSize; i++) {
		uint8_t databyte = readByte(addresses[i]);
		if (databyte != 0xFF) {
			#ifdef RUNDIAGNOSTIC
			errorcode = PREVWRITTEN;
 			_troubleshoot(errorcode);
 			#endif
			return false;
		}
	}
	return true;
}
#endif

#ifdef RUNDIAGNOSTIC
//Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of this file.
void SPIFlash::_troubleshoot(uint8_t error) {
	
	switch (error) {
		case SUCCESS:
		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(SUCCESS, HEX);
		#else
		Serial.println("Action completed successfully");
		#endif
		break;

 		case CALLBEGIN:
 		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(CALLBEGIN, HEX);
		#else
 		Serial.println("*constructor_of_choice*.begin() was not called in void setup()");
		#endif
		break;

		case UNKNOWNCHIP:
		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(UNKNOWNCHIP, HEX);
		#else
		Serial.println("Unable to identify chip. Are you shure this is a Winbond Flash chip");
 		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with your chip type and I will try to add support to your chip");
		#endif

		break;

 		case UNKNOWNCAP:
 		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(UNKNOWNCAP, HEX);
		#else
 		Serial.println("Unable to identify capacity.");
 		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with your chip type and I will work on adding support to your chip");
		#endif
		break;

 		case CHIPBUSY:
 		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(CHIPBUSY, HEX);
		#else
 		Serial.println("Chip is busy.");
 		Serial.println("Make sure all pins have been connected properly");
 		Serial.print("If it still doesn't work, ");
 		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occured");
		#endif
		break;

 		case OUTOFBOUNDS:
 		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(OUTOFBOUNDS, HEX);
		#else
 		Serial.println("Page overflow has been disabled and the address called exceeds the memory");
		#endif
		break;

 		case CANTENWRITE:
 		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(CANTENWRITE, HEX);
		#else
 		Serial.println("Unable to Enable Writing to chip.");
 		Serial.println("Please make sure the HOLD & WRITEPROTECT pins are connected properly to VCC & GND respectively");
 		Serial.print("If you are still facing issues, ");
 		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occured");
		#endif
		break;

		case PREVWRITTEN:
 		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x0");
		Serial.println(PREVWRITTEN, HEX);
		#else
 		Serial.println("This sector already contains data.");
 		Serial.println("Please make sure the sectors being written to are erased.");
 		Serial.print("If you are still facing issues, ");
 		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occured");
		#endif
		break;

		default:
		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x");
		Serial.println(UNKNOWNERROR, HEX);
		#else
		Serial.println("Unknown error");
 		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occured");
		#endif
		break;
	}
}
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Public functions used for read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Identifies chip and establishes parameters
void SPIFlash::begin() {
	#if defined (__arm__) && defined (__SAM3X8E__)
	SPI.begin(csPin);
	SPI.setClockDivider(csPin, 21);
	SPI.setDataMode(csPin, SPI_MODE0);
	#endif
	_chipID();
}

//Returns capacity of chip
uint32_t SPIFlash::getCapacity() {
	return capacity;
}

//Returns maximum number of pages
uint32_t SPIFlash::getMaxPage() {
	return maxPage;
}


//Returns identifying name of the chip
uint16_t SPIFlash::getChipName() {
	return name;
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

//Gets the next available address for use. Has two variants:
//	A. Takes the size of the data as an argument and returns a 32-bit address
//	B. Takes a three variables, the size of the data and two other variables to return a page number value & an offset into.
// All addresses in the in the sketch must be obtained via this function or not at all.
// Variant A
uint32_t SPIFlash::getAddress(uint16_t size) {
	if (!_addressCheck(currentAddress)){
		#ifdef RUNDIAGNOSTIC
		errorcode = OUTOFBOUNDS;
 		_troubleshoot(errorcode);
 		#endif
 		return false;
	}
	else {
		uint32_t address = currentAddress;
		/*Serial.print("Current Address: ");
		Serial.println(currentAddress);*/
		currentAddress+=size;
		return address;
	}
}
// Variant B
bool SPIFlash::getAddress(uint16_t size, uint16_t &page_number, uint8_t &offset) {
	uint32_t address = getAddress(size);
	offset = (address >> 0);
	page_number = (address >> 8);
	return true;
}

//Function for returning the size of the string (only to be used for the getAddress() function)
uint16_t SPIFlash::sizeofStr(String &inputStr) {
	uint16_t inStrLen = inputStr.length() + 1;
	uint16_t size;

	//inputStr.toCharArray(inputChar, inStrLen);

	size=(sizeof(char)*inStrLen);
	size+=sizeof(inStrLen);

	return size;
}

// Reads a byte of data from a specific location in a page. 
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint8_t SPIFlash::readByte(uint32_t address, bool fastRead) {
	uint8_t data;
	if (!_prepRead(address))
		return false;
	else {
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		data = _readNextByte(NO_CONTINUE);
		_endProcess();
		return data;
	}
}
// Variant B
uint8_t SPIFlash::readByte(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);

	return readByte(address, fastRead);
}

// Reads a char of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
int8_t SPIFlash::readChar(uint32_t address, bool fastRead) {
	int8_t data;
	if (!_prepRead(address))
		return false;
	else {
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		data = _readNextByte(NO_CONTINUE);
		_endProcess();
		return data;
	}
}
// Variant B
int8_t SPIFlash::readChar(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readChar(address, fastRead);
}

// Reads an array of bytes starting from a specific location in a page.// Has two variants:
//	A. Takes three arguments 
//		1. address --> Any address from 0 to maxAddress
//		2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes four arguments
//		1. page --> Any page number from 0 to maxPage
//		2. offset --> Any offset within the page - from 0 to 255
//		3. data_buffer --> The array of bytes to be read from the flash memory - starting at the offset on the page indicated
//		4. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint8_t  SPIFlash::readByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead) {
	if (!_prepRead(address)) {

	}
	else {	
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t a = 0; a < bufferSize; a++) {
			if (a == (bufferSize-1))
				data_buffer[a] = _readNextByte(NO_CONTINUE);
			else
				data_buffer[a] = _readNextByte();
			_addressCheck(address++);
		}
		_endProcess();
	}
	return true;
}
// Variant B
uint8_t  SPIFlash::readByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readByteArray(address, data_buffer, bufferSize, fastRead);
}

// Reads an array of bytes starting from a specific location in a page.// Has two variants:
//	A. Takes three arguments 
//		1. address --> Any address from 0 to maxAddress
//		2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes four arguments
//		1. page --> Any page number from 0 to maxPage
//		2. offset --> Any offset within the page - from 0 to 255
//		3. data_buffer --> The array of bytes to be read from the flash memory - starting at the offset on the page indicated
//		4. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint8_t  SPIFlash::readCharArray(uint32_t address, char *data_buffer, uint16_t bufferSize, bool fastRead) {
	if (_prepRead(address)) {
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t a = 0; a < bufferSize; a++) {
			_addressCheck(address+a);
			if (a == (bufferSize-1))
				data_buffer[a] = _readNextByte(NO_CONTINUE);
			else
				data_buffer[a] = _readNextByte();
		}
		_endProcess();
		return true;
	}
	else
		return false;
}
// Variant B
uint8_t  SPIFlash::readCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t bufferSize, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readCharArray(address, data_buffer, bufferSize, fastRead);
}

// Reads an unsigned int of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint16_t SPIFlash::readWord(uint32_t address, bool fastRead) {
	union
	{
		byte b[sizeof(uint16_t)];
		uint16_t I;
	} data;
	if (!_prepRead(address))
		return false;
	else {	
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t i=0; i < (sizeof(int16_t)); i++) {
			if (i == (sizeof(uint16_t)-1))
				data.b[i] = _readNextByte(NO_CONTINUE);
			else
				data.b[i] = _readNextByte();
		}
		_endProcess();
		return data.I;
	}
}
// Variant B
uint16_t SPIFlash::readWord(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readWord(address, fastRead);
}

// Reads a signed int of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
int16_t SPIFlash::readShort(uint32_t address, bool fastRead) {
	union
	{
		byte b[sizeof(int16_t)];
		int16_t s;
	} data;
	if (!_prepRead(address))
		return false;
	else {	
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t i=0; i < (sizeof(int16_t)); i++) {
			if (i == (sizeof(int16_t)-1))
				data.b[i] = _readNextByte(NO_CONTINUE);
			else
				data.b[i] = _readNextByte();
		}
		_endProcess();
		return data.s;
	}
}
// Variant B
int16_t SPIFlash::readShort(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readShort(address, fastRead);
}

// Reads an unsigned long of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint32_t SPIFlash::readULong(uint32_t address, bool fastRead) {
	union
	{
		byte b[sizeof(uint32_t)];
		uint32_t l;
	} data;
	if (!_prepRead(address))
		return false;
	else {	
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t i=0; i < (sizeof(uint32_t)); i++) {
			if (i == (sizeof(uint32_t)-1))
				data.b[i] = _readNextByte(NO_CONTINUE);
			else
				data.b[i] = _readNextByte();
		}
		_endProcess();
		return data.l;
	}
}
// Variant B
uint32_t SPIFlash::readULong(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readULong(address, fastRead);
}

// Reads a signed long of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
int32_t SPIFlash::readLong(uint32_t address, bool fastRead) {
	union
	{
		byte b[sizeof(int32_t)];
		int32_t l;
	} data;
	if (!_prepRead(address))
		return false;
	else {	
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t i=0; i < (sizeof(int32_t)); i++) {
			if (i == (sizeof(int32_t)-1))
				data.b[i] = _readNextByte(NO_CONTINUE);
			else
				data.b[i] = _readNextByte();
		}
		_endProcess();
		return data.l;
	}
}
// Variant B
int32_t SPIFlash::readLong(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readLong(address, fastRead);
}

// Reads a signed long of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments - 
//		1. address --> Any address from 0 to maxAddress
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
float SPIFlash::readFloat(uint32_t address, bool fastRead) {
	union
	{
		byte b[(sizeof(float))];
		float f;
	} data;
	if (!_prepRead(address))
		return false;
	else {	
		if(!fastRead)
			_beginRead(address);
		else
			_beginFastRead(address);

		for (uint16_t i=0; i < (sizeof(float)); i++) {
			if (i == (sizeof(float)-1))
				data.b[i] = _readNextByte(NO_CONTINUE);
			else
				data.b[i] = _readNextByte();
		}
		_endProcess();
		return data.f;
	}
}
// Variant B
float SPIFlash::readFloat(uint16_t page_number, uint8_t offset, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);
	
	return readFloat(address, fastRead);
}

// Reads a string from a specific location on a page.
// Has two variants:
//	A. Takes three arguments 
//		1. address --> Any address from 0 to maxAddress
//		2. outputString --> String variable to write the output to
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes four arguments
//		1. page --> Any page number from 0 to maxPage
//		2. offset --> Any offset within the page - from 0 to 255
//		3. outputString --> String variable to write the output to
//		4. fastRead --> defaults to false - executes _beginFastRead() if set to true
// This function first reads a short from the address to figure out the size of the String object stored and
// then reads the String object data
// Variant A
bool SPIFlash::readStr(uint32_t address, String &outStr, bool fastRead) {
  if (!_prepRead(address))
		return false;
  uint16_t strLen;

  strLen = readShort(address);
  address+=(sizeof(strLen));

  char outputChar[strLen];

  readCharArray(address, outputChar, strLen, fastRead);

  outStr = String(outputChar);
  return true;
}
// Variant B
bool SPIFlash::readStr(uint16_t page_number, uint8_t offset, String &outStr, bool fastRead) {
  uint32_t address = _getAddress(page_number, offset);
  return readStr(address, outStr, fastRead);
}

// Reads a page of data into a page buffer. Takes three arguments - 
//  1. page --> Any page number from 0 to maxPage
//  2. data_buffer --> a data buffer to read the data into (This HAS to be an array of 256 bytes)
//	3. fastRead --> defaults to false - executes _beginFastRead() if set to true
bool  SPIFlash::readPage(uint16_t page_number, uint8_t *data_buffer, bool fastRead) {
	uint32_t address = _prepRead(page_number);
	
	if(!fastRead)
		_beginRead(address);
	else
		_beginFastRead(address);

	for (int a = 0; a < 256; ++a) {
		if (a == (255))
				data_buffer[a] = _readNextByte(NO_CONTINUE);
			else
				data_buffer[a] = _readNextByte();
	}
	_endProcess();
	return true;
}

// Writes a byte of data to a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One byte of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One byte of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeByte(uint32_t address, uint8_t data, bool errorCheck) {
	if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	_beginWrite(address);
	_writeNextByte(data, NO_CONTINUE);
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}
// Variant B
bool SPIFlash::writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeByte(address, data, errorCheck);
}

// Writes a char of data to a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One char of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One char of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeChar(uint32_t address, int8_t data, bool errorCheck) {
	if (!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	_beginWrite(address);
	_writeNextByte(data, NO_CONTINUE);
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);

}
// Variant B
bool SPIFlash::writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck) {
	uint32_t address = _prepWrite(page_number, offset);
	
	return writeChar(address, data, errorCheck);

}

// Writes an array of bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> An array of bytes to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> An array of bytes to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck) {
	if (!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data_buffer)))
		return false;
	#endif

	_beginWrite(address);

	for (int i = 0; i < bufferSize; i++) {
		_addressCheck(address+i);
		if (i == (bufferSize-1))
			_writeNextByte(data_buffer[i], NO_CONTINUE);
		else
			_writeNextByte(data_buffer[i]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data_buffer);
}
// Variant B
bool SPIFlash::writeByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeByteArray(address, data_buffer, bufferSize, errorCheck);
}

// Writes an array of bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> An array of chars to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> An array of chars to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeCharArray(uint32_t address, char *data_buffer, uint16_t bufferSize, bool errorCheck) {
	if (!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data_buffer)))
		return false;
	#endif

	_beginWrite(address);

	for (int i = 0; i < bufferSize; i++) {
		_addressCheck(address+i);
		if (i == (bufferSize-1))
			_writeNextByte(data_buffer[i], NO_CONTINUE);
		else
			_writeNextByte(data_buffer[i]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data_buffer);
}
// Variant B
bool SPIFlash::writeCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t bufferSize, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);
	
	return writeCharArray(address, data_buffer, bufferSize, errorCheck);
}

// Writes an unsigned int as two bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One unsigned int of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One unsigned int of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeWord(uint32_t address, uint16_t data, bool errorCheck) {
	if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	union
	{
		uint8_t b[sizeof(data)];
		uint16_t w;
	} var;
	var.w = data;

	_beginWrite(address);
	for (uint16_t j = 0; j < sizeof(data); j++) {
		if (j == (sizeof(data)-1))
  	  		_writeNextByte(var.b[j], NO_CONTINUE);
  		else
      		_writeNextByte(var.b[j]);
	}
	_endProcess();

		if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}
// Variant B
bool SPIFlash::writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeWord(address, data, errorCheck);
}

// Writes a signed int as two bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One signed int of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One signed int of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeShort(uint32_t address, int16_t data, bool errorCheck) {
	if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	union
	{
		uint8_t b[sizeof(data)];
		int16_t s;
	} var;
	var.s = data;
	_beginWrite(address);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		if (j == (sizeof(data)-1))
		_writeNextByte(var.b[j], NO_CONTINUE);
	else
		_writeNextByte(var.b[j]);
	}
	_endProcess();

	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}
// Variant B
bool SPIFlash::writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeShort(address, data, errorCheck);
}

// Writes an unsigned long as four bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One unsigned long of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One unsigned long of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeULong(uint32_t address, uint32_t data, bool errorCheck) {
	if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	union
	{
		uint8_t b[sizeof(data)];
		uint32_t l;
	} var;
	var.l = data;
	_beginWrite(address);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		if (j == (sizeof(data)-1))
		_writeNextByte(var.b[j], NO_CONTINUE);
	else
		_writeNextByte(var.b[j]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}
// Variant B
bool SPIFlash::writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeULong(address, data, errorCheck);
}

// Writes a signed long as four bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One signed long of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One signed long of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeLong(uint32_t address, int32_t data, bool errorCheck) {
	if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	union
	{
		uint8_t b[sizeof(data)];
		int32_t l;
	} var;
	var.l = data;
	_beginWrite(address);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		if (j == (sizeof(data)-1))
		_writeNextByte(var.b[j], NO_CONTINUE);
	else
		_writeNextByte(var.b[j]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}
// Variant B
bool SPIFlash::writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeLong(address, data, errorCheck);
}

// Writes a float as four bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. address --> Any address - from 0 to maxAddress
//  	2. data --> One float of data to be written to a particular location on a page
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//  	3. data --> One float of data to be written to a particular location on a page
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
bool SPIFlash::writeFloat(uint32_t address, float data, bool errorCheck) {
	if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data)))
		return false;
	#endif

	union
	{
		uint8_t b[sizeof(data)];
		float f;
	} var;
	var.f = data;
	_beginWrite(address);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		if (j == (sizeof(data)-1))
		_writeNextByte(var.b[j], NO_CONTINUE);
	else
		_writeNextByte(var.b[j]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, data);
}
// Variant B
bool SPIFlash::writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

	return writeFloat(address, data, errorCheck);
}

// Reads a string from a specific location on a page.
// Has two variants:
//	A. Takes two arguments -
//  	1. address --> Any address from 0 to maxAddress
//		2. inputString --> String variable to write the data from
//		3. errorCheck --> Turned on by default. Checks for writing errors
//	B. Takes four arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. inputString --> String variable to write the data from
//		4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// This function first writes the size of the string as an unsigned int to the address to figure out the size of the String object stored and
// then writes the String object data. Therefore it takes up two bytes more than the size of the String itself.
// Variant A
bool SPIFlash::writeStr(uint32_t address, String &inputStr, bool errorCheck) {
  if(!_prepWrite(address))
		return false;

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(inputStr)))
		return false;
	#endif

  uint16_t inStrLen = inputStr.length() + 1;
  char inputChar[inStrLen];
  writeWord(address, inStrLen, errorCheck);

  address+=sizeof(inStrLen);
  
  inputStr.toCharArray(inputChar, inStrLen);
  writeCharArray(address, inputChar, inStrLen, errorCheck);
  return true;
}
// Variant B
bool SPIFlash::writeStr(uint16_t page_number, uint8_t offset, String &inputStr, bool errorCheck) {
  uint32_t address = _getAddress(page_number, offset);
  return writeStr(address, inputStr, errorCheck);
}

// Writes a page of data from a data_buffer array. Make sure the sizeOf(uint8_t data_buffer[]) == 256. 
//	errorCheck --> Turned on by default. Checks for writing errors.
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck) {
	uint32_t address = _prepWrite(page_number);

	#ifndef HIGHSPEED
	if(!_notPrevWritten(address, sizeof(data_buffer)))
		return false;
	#endif

	_beginWrite(address);
	for (uint16_t i = 0; i < PAGESIZE; ++i){
		if (i == (PAGESIZE-1))
		_writeNextByte(data_buffer[i], NO_CONTINUE);
	else
		_writeNextByte(data_buffer[i]);
	}
	_endProcess();
	
	if (!errorCheck)
		return true;
	else
		return _writeErrorCheck(address, *data_buffer);
}


//Erases one 4k sector. Has two variants:
//	A. Takes the address as the argument and erases the sector containing the address.
//	B. Takes page to be erased as the argument and erases the sector containing the page.
//	The sectors are numbered 0 - 255 containing 16 pages each. 
//			Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255
// Variant A
bool SPIFlash::eraseSector(uint32_t address) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	_cmd(SECTORERASE);
	#if defined (__arm__) && defined (__SAM3X8E__)
 	SPI.transfer(csPin, address >> 16, SPI_CONTINUE);
 	SPI.transfer(csPin, address >> 8, SPI_CONTINUE);
 	SPI.transfer(csPin, 0x00);
	#else
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	CHIP_DESELECT
	#endif

	if(!_notBusy(410L))
		return false;	//Datasheet says erasing a sector takes 400ms max

		//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
		// i.e. to write disable state upon the following conditions:
		// Power-up, Write Disable, Page Program, Quad Page Program, ``Sector Erase``, Block Erase, Chip Erase, Write Status Register,
		// Erase Security Register and Program Security register

	return true;
}
// Variant B
bool SPIFlash::eraseSector(uint16_t page_number, uint8_t offset) {
	uint32_t address = _getAddress(page_number, offset);
	return eraseSector(address);
}

//Erases one 32k block. Has two variants:
//	A. Takes the address as the argument and erases the block containing the address.
//	B. Takes page to be erased as the argument and erases the block containing the page.
//	The blocks are numbered 0 - 31 containing 128 pages each.
// 			Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31
// Variant A
bool SPIFlash::eraseBlock32K(uint32_t address) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	_cmd(BLOCK32ERASE);
	#if defined (__arm__) && defined (__SAM3X8E__)
 	SPI.transfer(csPin, address >> 16, SPI_CONTINUE);
 	SPI.transfer(csPin, address >> 8, SPI_CONTINUE);
 	SPI.transfer(csPin, 0x00);
	#else
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	CHIP_DESELECT
	#endif

	if(!_notBusy(410L))
	return false;	//Datasheet says erasing a sector takes 400ms max
	
	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}
// Variant B
bool SPIFlash::eraseBlock32K(uint16_t page_number, uint8_t offset) {
	uint32_t address = _getAddress(page_number, offset);
	return eraseBlock32K(address);
}

//Erases one 64k block. Has two variants:
//	A. Takes the address as the argument and erases the block containing the address.
//	B. Takes page to be erased as the argument and erases the block containing the page.
//	The blocks are numbered 0 - 15 containing 256 pages each.
// 				Page 0-255 --> Block 0; Page 256-511 --> Block 1;......Page 3840-4095 --> Block 15
//	Variant A
bool SPIFlash::eraseBlock64K(uint32_t address) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	_cmd(BLOCK64ERASE);
	#if defined (__arm__) && defined (__SAM3X8E__)
 	SPI.transfer(csPin, address >> 16, SPI_CONTINUE);
 	SPI.transfer(csPin, address >> 8, SPI_CONTINUE);
 	SPI.transfer(csPin, 0x00);
	#else
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	CHIP_DESELECT
	#endif

	if(!_notBusy(410L))
		return false;	//Datasheet says erasing a sector takes 400ms max

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}
//	Variant B
bool SPIFlash::eraseBlock64K(uint16_t page_number, uint8_t offset) {
	uint32_t address = _getAddress(page_number, offset);
	return eraseBlock64K(address);
}

//Erases whole chip. Think twice before using.
bool SPIFlash::eraseChip(void) {
	if(!_notBusy()||!_writeEnable())
 		return false;

	_cmd(CHIPERASE, NO_CONTINUE);
	#if defined (__AVR__)
	CHIP_DESELECT
	#endif

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
	if(_notBusy() || !_noSuspend())
		return false;

	_cmd(SUSPEND, NO_CONTINUE);
	#if defined (__AVR__)
	CHIP_DESELECT
	#endif

	_delay_us(20);

	if(!_notBusy() || _noSuspend())	//Max suspend Enable time according to datasheet
		return false;
	return true;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {
	if(!_notBusy() || _noSuspend())
		return false;

	_cmd(RESUME, NO_CONTINUE);
	#if defined (__AVR__)
	CHIP_DESELECT
	#endif

	_delay_us(20);

	if(_notBusy() || !_noSuspend())
		return false;
	return true;

}

//Puts device in low power state. Good for battery powered operations.
//Typical current consumption during power-down is 1mA with a maximum of 5mA. (Datasheet 7.4)
//In powerDown() the chip will only respond to powerUp()
bool SPIFlash::powerDown(void) {
	if(!_notBusy())
		return false;

	_cmd(POWERDOWN, NO_CONTINUE);
	#if defined (__AVR__)
	CHIP_DESELECT
	#endif
	_delay_us(3);							//Max powerDown enable time according to the Datasheet

	uint8_t status1 = _readStat1();
	uint8_t status2 = _readStat1();
	status1 = _readStat1();
	
	if (status1 != 255 && status2 != 255) {
		if (status1 == status2 || status1 == 0 || status2 == 0) {
			status1 = _readStat1();
			status2 = _readStat1();
		}
		else if (status1 != status2)
			return true;
	}
	else if (status1 == 255 && status2 == 255)
		return true;
	else if (status1 == 0 && status2 == 0)
		return false;
	return true;
}

//Wakes chip from low power state.
bool SPIFlash::powerUp(void) {

	_cmd(RELEASE, NO_CONTINUE);	
	#if defined (__AVR__)
	CHIP_DESELECT
	#endif
	_delay_us(3);						    //Max release enable time according to the Datasheet

	if (_readStat1() == 255)
		return false;
	return true;
}

/************************************************************************************************************************
		These functions are deprecated to enable compatibility with various AVR chips. They can be used by uncommenting 
		this block of code. However, be warned, this particular block of code has only been tested with the Arduino
		IDE (1.6.5) and only with 8-bit AVR based Arduino boards and is not supported from library version 2.0 onward
					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
					//        These functions work with data to/from Serial stream. 	  //
					//         Declares Serial.begin() if not previously declared.        //
					//                 Initiates Serial at 115200 baud.                   //
					//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


		This function is deprecated to enable compatibility with various AVR chips. It can be used by uncommenting 
		this block of code. However, be warned, this particular block of code has only been tested with the Arduino
		IDE and only with 8-bit AVR based Arduino boards.

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

//Reads a string from Serial
bool SPIFlash::readSerialStr(String &inputStr) {
	if(!Serial)
		Serial.begin(115200);
	while (Serial.available()) {
      inputStr = Serial.readStringUntil('\n');
      return true;
  }
  return false;
}

//Reads a page of data and prints it to Serial stream. Make sure the sizeOf(uint8_t data_buffer[]) == 256.
void SPIFlash::printPage(uint16_t page_number, uint8_t outputType, bool fastRead) {
	if(!Serial)
		Serial.begin(115200);

	char buffer[24];
	sprintf(buffer, "Reading page (%04x)", page_number);
	Serial.println(buffer);

	uint8_t data_buffer[256];
	readPage(page_number, data_buffer, fastRead);
	_printPageBytes(data_buffer, outputType);
}

//Reads all pages on Flash chip and dumps it to Serial stream. 
//This function is useful when extracting data from a flash chip onto a computer as a text file.
void SPIFlash::printAllPages(uint8_t outputType, bool fastRead) {
	if(!Serial)
		Serial.begin(115200);

	Serial.println("Reading all pages");
	uint8_t data_buffer[256];

	for (int a = 0; a < maxPage; a++) {
		readPage(a, data_buffer, fastRead);
		_printPageBytes(data_buffer, outputType);
  }
}
************************************************************************************************************************/