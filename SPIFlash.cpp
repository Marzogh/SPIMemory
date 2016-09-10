/* Arduino SPIFlash Library v.2.4.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 11/09/2016
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

#if defined (ARDUINO_ARCH_SAM) || defined (ARDUINO_ARCH_SAMD) || defined (ARDUINO_ARCH_ESP8266)
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
//#define HIGHSPEED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#if defined (ARDUINO_ARCH_AVR)
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
#elif defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_SAMD) || defined (ARDUINO_ARCH_SAM)
        #include <SPI.h>
        #define CHIP_SELECT   digitalWrite(csPin, LOW);
        #define CHIP_DESELECT digitalWrite(csPin, HIGH);
        #define xfer(n)   SPI.transfer(n)
#endif

// Constructor
#if defined (ARDUINO_ARCH_AVR)
SPIFlash::SPIFlash(uint8_t cs, bool overflow) {
	csPin = cs;
#ifndef __AVR_ATtiny85__
	cs_port = portOutputRegister(digitalPinToPort(csPin));
#endif
	cs_mask = digitalPinToBitMask(csPin);
    pageOverflow = overflow;
    pinMode(cs, OUTPUT);
}
#elif defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_SAMD) || defined (ARDUINO_ARCH_SAM)
SPIFlash::SPIFlash(uint8_t cs, bool overflow) {
	csPin = cs;
    pageOverflow = overflow;
    pinMode(cs, OUTPUT);
}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Private functions used by read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Double checks all parameters before calling a read or write. Comes in two variants
//Variant A: Takes address and returns the address if true, else returns false. Throws an error if there is a problem.
bool SPIFlash::_prep(uint8_t opcode, uint32_t address, uint32_t size) {
  switch (opcode) {
    case PAGEPROG:
    if (!_addressCheck(address, size)) {
      return false;
    }
    if(!_notBusy() || !_writeEnable()){
      return false;
    }
    #ifndef HIGHSPEED
    if(!_notPrevWritten(address, size)) {
      return false;
    }
    #endif
    return true;
    break;

    default:
    if (!_addressCheck(address, size)) {
      return false;
    }
    if (!_notBusy()){
      return false;
    }
    return true;
    break;
  }
}

//Variant B: Take the opcode, page number, offset and size of data block as arguments
bool SPIFlash::_prep(uint8_t opcode, uint32_t page_number, uint8_t offset, uint32_t size) {
  uint32_t address = _getAddress(page_number, offset);
  return _prep(opcode, address, size);
}

bool SPIFlash::_transferAddress(void) {
  #ifdef __AVR_ATtiny85__
  (void)xfer(_currentAddress >> 16);
  (void)xfer(_currentAddress >> 8);
  (void)xfer(_currentAddress);
  #else
  SPI.transfer(_currentAddress >> 16);
  SPI.transfer(_currentAddress >> 8);
  SPI.transfer(_currentAddress);
  #endif
}

//Initiates SPI operation - but data is not transferred yet. Always call _prep() before this function (especially when it involved writing or reading to/from an address)
bool SPIFlash::_beginSPI(uint8_t opcode) {
  switch (opcode) {
    case READDATA:
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    //SPI.setClockDivider(SPI_CLOCK_DIV4); //Uncomment this if more than one SPI device is presaent on the bus
    SPI.begin();
    CHIP_SELECT
    #ifdef __AVR_ATtiny85__
    (void)xfer(opcode);
    #else
    SPI.transfer(opcode);
    #endif
    _transferAddress();
    break;

    case FASTREAD:
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    //SPI.setClockDivider(SPI_CLOCK_DIV4); //Uncomment this if more than one SPI device is presaent on the bus
    SPI.begin();
    CHIP_SELECT
    #ifdef __AVR_ATtiny85__
    (void)xfer(opcode);
    #else
    SPI.transfer(opcode);
    #endif
    _transferAddress();
    break;

    case PAGEPROG:
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    //SPI.setClockDivider(SPI_CLOCK_DIV4); //Uncomment this if more than one SPI device is presaent on the bus
    SPI.begin();
    CHIP_SELECT
    #ifdef __AVR_ATtiny85__
    (void)xfer(opcode);
    #else
    SPI.transfer(opcode);
    #endif
    _transferAddress();
    break;

    default:
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    //SPI.setClockDivider(SPI_CLOCK_DIV4); //Uncomment this if more than one SPI device is presaent on the bus
    SPI.begin();
    CHIP_SELECT
    #ifdef __AVR_ATtiny85__
    (void)xfer(opcode);
    #else
    SPI.transfer(opcode);
    #endif
    break;
  }
  return true;
}
//SPI data lines are left open until _endSPI() is called

//Reads/Writes next byte. Call 'n' times to read/write 'n' number of bytes. Should be called after _begin()
uint8_t SPIFlash::_nextByte(uint8_t opcode, uint8_t data) {
  switch (opcode) {
    case READDATA:
    uint8_t result;
    #ifdef __AVR_ATtiny85__
    result = xfer(NULLBYTE);
    #else
    	result = SPI.transfer(NULLBYTE);
    #endif
    return result;
    break;

    case PAGEPROG:
    #ifdef __AVR_ATtiny85__
    xfer(data);
    #else
    SPI.transfer(data);
    #endif
    return true;
    break;

    default:
    return false;
    break;
  }
}

//Stops all operations. Should be called after all the required data is read/written from repeated _readNextByte()/_nextByte(PAGEPROG, ) calls
void SPIFlash::_endSPI(void) {
  CHIP_DESELECT
}

// Checks if status register 1 can be accessed - used during powerdown and power up and for debugging
uint8_t SPIFlash::_readStat1(void) {
	_beginSPI(READSTAT1);
  uint8_t stat1 = _nextByte(READDATA);
  _endSPI();
	return stat1;
}

// Checks if status register 2 can be accessed, if yes, reads and returns it
uint8_t SPIFlash::_readStat2(void) {
  _beginSPI(READSTAT2);
  uint8_t stat2 = _nextByte(READDATA);
  _endSPI();
  return stat2;
}

// Checks the erase/program suspend flag before enabling/disabling a program/erase suspend operation
bool SPIFlash::_noSuspend(void) {
	if(_readStat2() & SUS)
		return false;
	return true;
}

// Polls the status register 1 until busy flag is cleared or timeout
bool SPIFlash::_notBusy(uint32_t timeout) {
	uint32_t startTime = millis();

	do {
    state = _readStat1();
		if((millis()-startTime) > timeout){
      errorcode = CHIPBUSY;
			#ifdef RUNDIAGNOSTIC
			_troubleshoot();
			#endif
			return false;
		}
	} while(state & BUSY);
	return true;
}

//Enables writing to chip by setting the WRITEENABLE bit
bool SPIFlash::_writeEnable(uint32_t timeout) {
  uint32_t startTime = millis();
  if (!(state & WRTEN)) {
    do {
      _beginSPI(WRITEENABLE);
      _endSPI();
      state = _readStat1();
      if((millis()-startTime) > timeout) {
        errorcode = CANTENWRITE;
        #ifdef RUNDIAGNOSTIC
        _troubleshoot();
        #endif
        return false;
       }
     } while (!(state & WRTEN));
  }
  return true;
}

//Disables writing to chip by setting the Write Enable Latch (WEL) bit in the Status Register to 0
//_writeDisable() is not required under the following conditions because the Write Enable Latch (WEL) flag is cleared to 0
// i.e. to write disable state:
// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register,
// Erase Security Register and Program Security register
bool SPIFlash::_writeDisable(void) {
	_beginSPI(WRITEDISABLE);
  _endSPI();
	return true;
}

//Gets address from page number and offset. Takes two arguments:
// 1. page_number --> Any page number from 0 to maxPage
// 2. offset --> Any offset within the page - from 0 to 255
uint32_t SPIFlash::_getAddress(uint16_t page_number, uint8_t offset) {
	uint32_t address = page_number;
	return ((address << 8) + offset);
}

//Checks the device ID to establish storage parameters
bool SPIFlash::_getManId(uint8_t *b1, uint8_t *b2) {
	if(!_notBusy())
		return false;
	_beginSPI(MANID);
  _nextByte(READDATA);
  _nextByte(READDATA);
  _nextByte(READDATA);
  *b1 = _nextByte(READDATA);
  *b2 = _nextByte(READDATA);
  _endSPI();
	return true;
}

//Checks for presence of chip by requesting JEDEC ID
bool SPIFlash::_getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3) {
  if(!_notBusy())
  	return false;
  _beginSPI(JEDECID);
	*b1 = SPI.transfer(READDATA);		// manufacturer id
	*b2 = SPI.transfer(READDATA);		// manufacturer id
	*b3 = SPI.transfer(READDATA);					// capacity
  _endSPI();
  return true;
}

//Identifies the chip
bool SPIFlash::_chipID(void) {
	//Get Manfucturer/Device ID so the library can identify the chip
    uint8_t manID, capID, devID ;
    //_getManId(&manID, &devID);
    _getJedecId(&manID, &capID, &devID);
    //Serial.println(manID, HEX);
    //Serial.println(capID, HEX);
    //Serial.println(devID, HEX);

    if (manID != WINBOND_MANID && manID != MICROCHIP_MANID){		//If the chip is not a Winbond Chip
      errorcode = UNKNOWNCHIP;		//Error code for unidentified chip
    	#ifdef RUNDIAGNOSTIC
    	_troubleshoot();
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
        //Serial.println(devID, HEX);
        //Serial.println(capacity);
        //Serial.println(name);
    	}
    }
    if (capacity == 0) {
      errorcode = UNKNOWNCAP;		//Error code for unidentified capacity
    	#ifdef RUNDIAGNOSTIC
    	_troubleshoot();
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
bool SPIFlash::_addressCheck(uint32_t address, uint32_t size) {
	if (capacity == 0) {
    errorcode = CALLBEGIN;
    #ifdef RUNDIAGNOSTIC
    _troubleshoot();
    #endif
	}

  for (uint32_t i = 0; i < size; i++) {
    if (address + i >= maxAddress) {
    	if (!pageOverflow) {
        errorcode = OUTOFBOUNDS;
        #ifdef RUNDIAGNOSTIC
        _troubleshoot();
        #endif
        return false;					// At end of memory - (!pageOverflow)
      }
      else {
        _currentAddress = 0x00;
        return true;					// At end of memory - (pageOverflow)
      }
    }
  }
  _currentAddress = address;
  return true;				// Not at end of memory if (address < capacity)
}

bool SPIFlash::_notPrevWritten(uint32_t address, uint32_t size) {
  //_prep(READDATA, address, size);
  _beginSPI(READDATA);
  for (uint16_t i = 0; i < size; i++) {
    if (_nextByte(READDATA) != 0xFF) {
      _endSPI();
      return false;
    }
  }
  _endSPI();
  return true;
}

#ifdef RUNDIAGNOSTIC
//Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of this file.
void SPIFlash::_troubleshoot() {

	switch (errorcode) {
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
		Serial.println("Unable to identify chip. Are you sure this is a Winbond Flash chip");
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
 		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
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
 		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
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
 		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
		#endif
		break;

		default:
		#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega32U4__) || defined (__AVR_ATtiny85__)
 		Serial.print("Error code: 0x");
		Serial.println(UNKNOWNERROR, HEX);
		#else
		Serial.println("Unknown error");
 		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
		#endif
		break;
	}
}
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Public functions used for read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Identifies chip and establishes parameters
void SPIFlash::begin(void) {
	_chipID();
}

uint8_t SPIFlash::error() {
	return errorcode;
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

//Checks for and initiates the chip by requesting the Manufacturer ID which is returned as a 16 bit int
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
	if (!_addressCheck(currentAddress, size)){
    errorcode = OUTOFBOUNDS;
    #ifdef RUNDIAGNOSTIC
    _troubleshoot();
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
	//uint16_t inStrLen = inputStr.length() + 1;
	uint16_t size;

	//inputStr.toCharArray(inputChar, inStrLen);

	//size=(sizeof(char)*inStrLen);
  size = (sizeof(char)*(inputStr.length()+1));
	size+=sizeof(inputStr.length()+1/*inStrLen*/);

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
	if (!_prep(READDATA, address, sizeof(data))) {
		return false;
  }

  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

  data = _nextByte(READDATA);
  _endSPI();
  return data;
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
	if (!_prep(READDATA, address, sizeof(data))) {
		return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

  data = _nextByte(READDATA);
  _endSPI();
  return data;
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
bool  SPIFlash::readByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead) {
	if (!_prep(READDATA, address, bufferSize)) {
    return false;
	}
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }
  for (uint16_t a = 0; a < bufferSize; a++) {
    data_buffer[a] = _nextByte(READDATA);
  }
  _endSPI();
	return true;
}
// Variant B
bool  SPIFlash::readByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead) {
	uint32_t address = _getAddress(page_number, offset);

	return readByteArray(address, data_buffer, bufferSize, fastRead);
}

// Reads an array of chars starting from a specific location in a page.// Has two variants:
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
bool  SPIFlash::readCharArray(uint32_t address, char *data_buffer, uint16_t bufferSize, bool fastRead) {
	if (!_prep(READDATA, address, bufferSize)) {
    return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

  for (uint16_t a = 0; a < bufferSize; a++) {
				data_buffer[a] = _nextByte(READDATA);
		}

	_endSPI();
	return true;
}
// Variant B
bool  SPIFlash::readCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t bufferSize, bool fastRead) {
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

	if (!_prep(READDATA, address, sizeof(data.I))) {
		return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

	for (uint16_t i=0; i < (sizeof(data.I)); i++) {
		data.b[i] = _nextByte(READDATA);
  }

  _endSPI();
	return data.I;
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
	if (!_prep(READDATA, address, sizeof(data.s))) {
    return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

  for (uint16_t i=0; i < (sizeof(data.s)); i++) {
		data.b[i] = _nextByte(READDATA);
	}
	_endSPI();
	return data.s;
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

	if (!_prep(READDATA, address, sizeof(data.l))) {
    return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }
	for (uint16_t i=0; i < (sizeof(data.l)); i++) {
		data.b[i] = _nextByte(READDATA);
	}
	_endSPI();
	return data.l;
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

	if (!_prep(READDATA, address, sizeof(data.l))) {
    return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

  for (uint16_t i=0; i < (sizeof(data.l)); i++) {
		data.b[i] = _nextByte(READDATA);
	}

	_endSPI();
	return data.l;
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

	if (!_prep(READDATA, address, sizeof(data.f))) {
    return false;
  }

  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

	for (uint16_t i=0; i < (sizeof(float)); i++) {
		data.b[i] = _nextByte(READDATA);
	}
	_endSPI();
	return data.f;
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
  uint16_t strLen;

  strLen = readShort(address);
  address+=(sizeof(strLen));
  /*if (!_prep(READDATA, address, (strLen + sizeof(strLen)))) {
    return false;
  }*/
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
	uint32_t address = _getAddress(page_number);

  if(!_prep(READDATA, address, PAGESIZE)) {
    return false;
  }
  switch (fastRead) {
    case false:
    _beginSPI(READDATA);
    break;

    case true:
    _beginSPI(FASTREAD);
    break;

    default:
    break;
  }

	for (int a = 0; a < PAGESIZE; a++) {
		data_buffer[a] = _nextByte(READDATA);
	}
	_endSPI();
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
	if(!_prep(PAGEPROG, address, sizeof(data))) {
    return false;
  }
	_beginSPI(PAGEPROG);
	_nextByte(PAGEPROG, data);
	_endSPI();

	switch (errorCheck) {
    case true:
    return _writeErrorCheck(address, data);
    break;

    case false:
    return true;
    break;
  }
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
	if (!_prep(PAGEPROG, address, sizeof(data)))
		return false;

	_beginSPI(PAGEPROG);
	_nextByte(PAGEPROG, data);
	_endSPI();

	if (!errorCheck) {
		return true;
  }
	else {
		return _writeErrorCheck(address, data);
  }
}
// Variant B
bool SPIFlash::writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck) {
	uint32_t address = _getAddress(page_number, offset);

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
	if (!_prep(PAGEPROG, address, bufferSize))
		return false;

	_beginSPI(PAGEPROG);

	for (uint16_t i = 0; i < bufferSize; i++) {
		_nextByte(PAGEPROG, data_buffer[i]);
	}
	_endSPI();

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
	if (!_prep(PAGEPROG, address, bufferSize))
		return false;

	_beginSPI(PAGEPROG);

	for (uint16_t i = 0; i < bufferSize; i++) {
		_nextByte(PAGEPROG, data_buffer[i]);
	}
	_endSPI();

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
	if(!_prep(PAGEPROG, address, sizeof(data)))
		return false;

	union
	{
		uint8_t b[sizeof(data)];
		uint16_t w;
	} var;
	var.w = data;

	_beginSPI(PAGEPROG);
	for (uint16_t j = 0; j < sizeof(data); j++) {
		_nextByte(PAGEPROG, var.b[j]);
	}
	_endSPI();

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
	if(!_prep(PAGEPROG, address, sizeof(data)))
		return false;

	union
	{
		uint8_t b[sizeof(data)];
		int16_t s;
	} var;
	var.s = data;
	_beginSPI(PAGEPROG);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
    _nextByte(PAGEPROG, var.b[j]);
	}
	_endSPI();

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
	if(!_prep(PAGEPROG, address, sizeof(data)))
		return false;

	union
	{
		uint8_t b[sizeof(data)];
		uint32_t l;
	} var;
	var.l = data;
	_beginSPI(PAGEPROG);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		_nextByte(PAGEPROG, var.b[j]);
	}
	_endSPI();

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
	if(!_prep(PAGEPROG, address, sizeof(data)))
		return false;

	union
	{
		uint8_t b[sizeof(data)];
		int32_t l;
	} var;
	var.l = data;
	_beginSPI(PAGEPROG);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		_nextByte(PAGEPROG, var.b[j]);
	}
	_endSPI();

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
	if(!_prep(PAGEPROG, address, sizeof(data)))
		return false;

	union
	{
		uint8_t b[sizeof(data)];
		float f;
	} var;
	var.f = data;
	_beginSPI(PAGEPROG);
	for (uint16_t j = 0; j < (sizeof(data)); j++) {
		_nextByte(PAGEPROG, var.b[j]);
	}
	_endSPI();

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
  uint16_t inStrLen = inputStr.length() +1;
  char inputChar[inStrLen];

  union
  {
    uint8_t b[sizeof(inStrLen)];
    uint16_t w;
  }var;
  var.w = inStrLen;
  inputStr.toCharArray(inputChar, inStrLen);

  if(!_prep(PAGEPROG, address, inStrLen))
		return false;

  _beginSPI(PAGEPROG);

  for (uint16_t j = 0; j < sizeof(inStrLen); j++) {
    _nextByte(PAGEPROG, var.b[j]);
  }

  for (uint16_t i = 0; i <inStrLen; i++) {
    _nextByte(PAGEPROG, inputChar[i]);
  }
  _endSPI();

  if (!errorCheck) {
    return true;
  }
  else {
    String tempStr;
    readStr(address, tempStr);

    return inputStr.equals(tempStr);
  }
}
// Variant B
bool SPIFlash::writeStr(uint16_t page_number, uint8_t offset, String &inputStr, bool errorCheck) {
  uint32_t address = _getAddress(page_number, offset);
  return writeStr(address, inputStr, errorCheck);
}

// Writes a page of data from a data_buffer array. Make sure the sizeOf(uint8_t data_buffer[]) >= PAGESIZE.
//	errorCheck --> Turned on by default. Checks for writing errors.
// WARNING: You can only write to previously erased memory locations (see datasheet).
// 			Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writePage(uint16_t page_number, const uint8_t *data_buffer, bool errorCheck) {
	uint32_t address = _getAddress(page_number);
  if (!_prep(PAGEPROG, address, PAGESIZE)) {
    return false;
  }

	_beginSPI(PAGEPROG);
	for (uint16_t i = 0; i < PAGESIZE; i++){
		_nextByte(PAGEPROG, data_buffer[i]);
	}
	_endSPI();

	if (!errorCheck)
		return true;
	else {
    _beginSPI(READDATA);
    for (uint16_t j = 0; j < PAGESIZE; j++){
      if (data_buffer[j] != _nextByte(READDATA)) {
          return false;
        }
      }
    _endSPI();
    return true;
  }
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

	_beginSPI(SECTORERASE);
	#ifdef __AVR_ATtiny85__
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	#else
 	SPI.transfer(address >> 16);
 	SPI.transfer(address >> 8);
 	SPI.transfer(NULLBYTE);
	#endif
  _endSPI();

	if(!_notBusy(500L))
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
	if(!_notBusy()||!_writeEnable()) {
 		return false;
  }
  _beginSPI(BLOCK32ERASE);
	#ifdef __AVR_ATtiny85__
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	#else
 	SPI.transfer(address >> 16);
 	SPI.transfer(address >> 8);
 	SPI.transfer(NULLBYTE);
	#endif
  _endSPI();

	if(!_notBusy(1000L))
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
	if(!_notBusy()||!_writeEnable()) {
 		return false;
  }
  _beginSPI(BLOCK64ERASE);
	#ifdef __AVR_ATtiny85__
	(void)xfer(address >> 16);
	(void)xfer(address >> 8);
	(void)xfer(0);
	#else
 	SPI.transfer(address >> 16);
 	SPI.transfer(address >> 8);
 	SPI.transfer(NULLBYTE);
	#endif
  _endSPI();

	if(!_notBusy(1200L))
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

	_beginSPI(CHIPERASE);
	_endSPI();

	if(!_notBusy(50000L))
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

	_beginSPI(SUSPEND);
  _endSPI();

	_delay_us(20);

	if(!_notBusy(50) || _noSuspend())	//Max suspend Enable time according to datasheet
		return false;
	return true;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {
	if(!_notBusy() || _noSuspend())
		return false;

	_beginSPI(RESUME);
	_endSPI();

	_delay_us(20);

	if(_notBusy(10) || !_noSuspend())
		return false;
	return true;

}

//Puts device in low power state. Good for battery powered operations.
//Typical current consumption during power-down is 1mA with a maximum of 5mA. (Datasheet 7.4)
//In powerDown() the chip will only respond to powerUp()
bool SPIFlash::powerDown(void) {
	if(!_notBusy(20))
		return false;

	_beginSPI(POWERDOWN);
  _endSPI();
	_delay_us(5);							//Max powerDown enable time according to the Datasheet

	uint8_t status1 = _readStat1();
	uint8_t status2 = _readStat1();
	status1 = _readStat1();

	if (status1 != 0xFF && status2 != 0xFF) {
		if (status1 == status2 || status1 == 0x00 || status2 == 0x00) {
			status1 = _readStat1();
			status2 = _readStat1();
		}
		else if (status1 != status2)
			return true;
	}
	else if (status1 == 0xFF && status2 == 0xFF)
		return true;
	else if (status1 == 0x00 && status2 == 0x00)
		return false;
	return true;
}

//Wakes chip from low power state.
bool SPIFlash::powerUp(void) {
	_beginSPI(RELEASE);
  _endSPI();
	_delay_us(3);						    //Max release enable time according to the Datasheet

	if (_readStat1() == 0xFF)
		return false;
	return true;
}
