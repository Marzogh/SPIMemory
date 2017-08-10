/* Arduino SPIFlash Library v.3.0.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 09/08/2017
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

// Constructor
//If board has multiple SPI interfaces, this constructor lets the user choose between them
// Adding Low level HAL API to initialize the Chip select pinMode on RTL8195A - @boseji <salearj@hotmail.com> 2nd March 2017
#if defined (ARDUINO_ARCH_AVR)
SPIFlash::SPIFlash(uint8_t cs) {
  csPin = cs;
  cs_mask = digitalPinToBitMask(csPin);
  pinMode(csPin, OUTPUT);
}
#elif defined (ARDUINO_ARCH_SAMD)
SPIFlash::SPIFlash(uint8_t cs, SPIClass *spiinterface) {
  _spi = spiinterface;  //Sets SPI interface - if no user selection is made, this defaults to SPI
  csPin = cs;
  pinMode(csPin, OUTPUT);
}
#elif defined (BOARD_RTL8195A)
SPIFlash::SPIFlash(PinName cs) {
  gpio_init(&csPin, cs);
  gpio_dir(&csPin, PIN_OUTPUT);
  gpio_mode(&csPin, PullNone);
  gpio_write(&csPin, 1);
}
#else
SPIFlash::SPIFlash(uint8_t cs) {
  csPin = cs;
  pinMode(csPin, OUTPUT);
}
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Public functions used for read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Identifies chip and establishes parameters
bool SPIFlash::begin(void) {
#if defined (CHIPSIZE)
    _chip.capacity = CHIPSIZE;
#endif
  BEGIN_SPI
#ifdef SPI_HAS_TRANSACTION
  //Define the settings to be used by the SPI bus
  _settings = SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0);
#endif
  return _chipID();
}

//Allows the setting of a custom clock speed for the SPI bus to communicate with the chip.
//Only works if the SPI library in use supports SPI Transactions
#ifdef SPI_HAS_TRANSACTION
void SPIFlash::setClock(uint32_t clockSpeed) {
  _settings = SPISettings(clockSpeed, MSBFIRST, SPI_MODE0);
}
#endif

uint8_t SPIFlash::error(bool _verbosity) {
  if (!_verbosity) {
    return errorcode;
  }
  else {
    _troubleshoot(errorcode, PRINTOVERRIDE);
    return errorcode;
  }
}

//Returns capacity of chip
uint32_t SPIFlash::getCapacity(void) {
	return _chip.capacity;
}

//Returns maximum number of pages
uint32_t SPIFlash::getMaxPage(void) {
	return (_chip.capacity / PAGESIZE);
}

//Returns the library version as a string
bool SPIFlash::libver(uint8_t *b1, uint8_t *b2, uint8_t *b3) {
  *b1 = LIBVER;
  *b2 = LIBSUBVER;
  *b3 = BUGFIXVER;
  return true;
}

//Checks for and initiates the chip by requesting the Manufacturer ID which is returned as a 16 bit int
uint16_t SPIFlash::getManID(void) {
	uint8_t b1, b2;
    _getManId(&b1, &b2);
    uint32_t id = b1;
    id = (id << 8)|(b2 << 0);
    return id;
}

//Returns JEDEC ID which is returned as a 32 bit int
uint32_t SPIFlash::getJEDECID(void) {
    uint32_t id = _chip.manufacturerID;
    id = (id << 8)|(_chip.memoryTypeID << 0);
    id = (id << 8)|(_chip.capacityID << 0);
    return id;
}

//Gets the next available address for use. Has two variants:
//	A. Takes the size of the data as an argument and returns a 32-bit address
//	B. Takes a three variables, the size of the data and two other variables to return a page number value & an offset into.
// All addresses in the in the sketch must be obtained via this function or not at all.
// Variant A
uint32_t SPIFlash::getAddress(uint16_t size) {
  if (!_addressCheck(currentAddress, size)){
    _troubleshoot(OUTOFBOUNDS);
    return false;
	}
	else {
		uint32_t _addr = currentAddress;
		currentAddress+=size;
		return _addr;
	}
}

//Function for returning the size of the string (only to be used for the getAddress() function)
uint16_t SPIFlash::sizeofStr(String &inputStr) {
  uint16_t size;
  size = (sizeof(char)*(inputStr.length()+1));
  size+=sizeof(inputStr.length()+1);

	return size;
}

// Reads a byte of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any address from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint8_t SPIFlash::readByte(uint32_t _addr, bool fastRead) {
  uint8_t data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads a char of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any address from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
int8_t SPIFlash::readChar(uint32_t _addr, bool fastRead) {
  int8_t data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads an array of bytes starting from a specific location in a page.// Has two variants:
//	A. Takes three arguments
//		1. _addr --> Any address from 0 to capacity
//		2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes four arguments
//		1. page --> Any page number from 0 to maxPage
//		2. offset --> Any offset within the page - from 0 to 255
//		3. data_buffer --> The array of bytes to be read from the flash memory - starting at the offset on the page indicated
//		4. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
bool  SPIFlash::readByteArray(uint32_t _addr, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead) {
	if (!_prep(READDATA, _addr, bufferSize)) {
    return false;
	}
  if(fastRead) {
    _beginSPI(FASTREAD);
  }
  else {
    _beginSPI(READDATA);
  }
  _nextBuf(READDATA, &(*data_buffer), bufferSize);
  _endSPI();
	return true;
}

// Reads an array of chars starting from a specific location in a page.// Has two variants:
//	A. Takes three arguments
//		1. _addr --> Any address from 0 to capacity
//		2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes four arguments
//		1. page --> Any page number from 0 to maxPage
//		2. offset --> Any offset within the page - from 0 to 255
//		3. data_buffer --> The array of bytes to be read from the flash memory - starting at the offset on the page indicated
//		4. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
bool  SPIFlash::readCharArray(uint32_t _addr, char *data_buffer, uint16_t bufferSize, bool fastRead) {
  if (!_prep(READDATA, _addr, bufferSize)) {
    return false;
	}
  if(fastRead) {
    _beginSPI(FASTREAD);
  }
  else {
    _beginSPI(READDATA);
  }
  _nextBuf(READDATA, (uint8_t*) &(*data_buffer), bufferSize);
  _endSPI();
	return true;
}

// Reads an unsigned int of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any address from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint16_t SPIFlash::readWord(uint32_t _addr, bool fastRead) {
  uint16_t data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads a signed int of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any address from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
int16_t SPIFlash::readShort(uint32_t _addr, bool fastRead) {
  int16_t data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads an unsigned long of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any _addr from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
uint32_t SPIFlash::readULong(uint32_t _addr, bool fastRead) {
  uint32_t data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads a signed long of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any _addr from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
int32_t SPIFlash::readLong(uint32_t _addr, bool fastRead) {
  int32_t data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads a signed long of data from a specific location in a page.
// Has two variants:
//	A. Takes two arguments -
//		1. _addr --> Any address from 0 to capacity
//		2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//	B. Takes three arguments -
//  	1. page --> Any page number from 0 to maxPage
//  	2. offset --> Any offset within the page - from 0 to 255
//		3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
float SPIFlash::readFloat(uint32_t _addr, bool fastRead) {
  float data;
  uint8_t _sizeofdata = sizeof(data);
  _read(_addr, data, _sizeofdata, fastRead);
  return data;
}

// Reads a string from a specific location on a page.
// Has two variants:
//	A. Takes three arguments
//		1. _addr --> Any address from 0 to capacity
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
bool SPIFlash::readStr(uint32_t _addr, String &outStr, bool fastRead) {
  uint8_t _sizeofdata = sizeof(outStr);
  return _read(_addr, outStr, _sizeofdata, fastRead);
}

// Writes a byte of data to a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeByte(uint32_t _addr, uint8_t data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Writes a char of data to a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeChar(uint32_t _addr, int8_t data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Writes an array of bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeByteArray(uint32_t _addr, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck) {
  if (!_prep(PAGEPROG, _addr, bufferSize)) {
    return false;
  }
  uint16_t maxBytes = PAGESIZE-(_addr % PAGESIZE);  // Force the first set of bytes to stay within the first page

  if (bufferSize <= maxBytes) {
    CHIP_SELECT
    _nextByte(PAGEPROG);
    _transferAddress();
    _nextBuf(PAGEPROG, &data_buffer[0], bufferSize);
    CHIP_DESELECT
  }
  else {
    uint16_t length = bufferSize;
    uint16_t writeBufSz;
    uint16_t data_offset = 0;

    do {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;

      CHIP_SELECT
      _nextByte(PAGEPROG);
      _transferAddress();
      for (uint16_t i = 0; i < writeBufSz; ++i) {
        _nextByte(data_buffer[data_offset + i]);
      }
      CHIP_DESELECT

      _currentAddress += writeBufSz;
      data_offset += writeBufSz;
      length -= writeBufSz;
      maxBytes = 256;   // Now we can do up to 256 bytes per loop

      if(!_notBusy() || !_writeEnable()){
        return false;
      }

    } while (length > 0);
  }

  if (!errorCheck) {
    _endSPI();
    return true;
  }
  else {
    if (!_notBusy()) {
      return false;
    }
    _currentAddress = _addr;
    CHIP_SELECT
    _nextByte(READDATA);
    _transferAddress();
    for (uint16_t j = 0; j < bufferSize; j++) {
      if (_nextByte() != data_buffer[j]) {
        return false;
      }
    }
    _endSPI();
    return true;
  }
}

// Writes an array of bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeCharArray(uint32_t _addr, char *data_buffer, uint16_t bufferSize, bool errorCheck) {
  if (!_prep(PAGEPROG, _addr, bufferSize)) {
    return false;
  }
  uint16_t maxBytes = PAGESIZE-(_addr % PAGESIZE);  // Force the first set of bytes to stay within the first page

  if (bufferSize <= maxBytes) {
    CHIP_SELECT
    _nextByte(PAGEPROG);
    _transferAddress();
    _nextBuf(PAGEPROG, (uint8_t*) &data_buffer[0], bufferSize);
    CHIP_DESELECT
  }
  else {
    uint16_t length = bufferSize;
    uint16_t writeBufSz;
    uint16_t data_offset = 0;

    do {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;

      CHIP_SELECT
      _nextByte(PAGEPROG);
      _transferAddress();
      for (uint16_t i = 0; i < writeBufSz; ++i) {
        _nextByte(data_buffer[data_offset + i]);
      }
      CHIP_DESELECT

      _currentAddress += writeBufSz;
      data_offset += writeBufSz;
      length -= writeBufSz;
      maxBytes = 256;   // Now we can do up to 256 bytes per loop

      if(!_notBusy() || !_writeEnable()){
        return false;
      }

    } while (length > 0);
  }

  if (!errorCheck) {
    _endSPI();
    return true;
  }
  else {
    if (!_notBusy()) {
      return false;
    }
    _currentAddress = _addr;
    CHIP_SELECT
    _nextByte(READDATA);
    _transferAddress();
    for (uint16_t j = 0; j < bufferSize; j++) {
      if (_nextByte() != data_buffer[j]) {
        return false;
      }
    }
    _endSPI();
    return true;
  }
}

// Writes an unsigned int as two bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeWord(uint32_t _addr, uint16_t data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Writes a signed int as two bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeShort(uint32_t _addr, int16_t data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Writes an unsigned long as four bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeULong(uint32_t _addr, uint32_t data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Writes a signed long as four bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeLong(uint32_t _addr, int32_t data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Writes a float as four bytes starting from a specific location in a page.
// Has two variants:
//	A. Takes three arguments -
//  	1. _addr --> Any address - from 0 to capacity
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
bool SPIFlash::writeFloat(uint32_t _addr, float data, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(data);
  return _write(_addr, data, _sizeofdata, errorCheck);
}

// Reads a string from a specific location on a page.
// Has two variants:
//	A. Takes two arguments -
//  	1. _addr --> Any address from 0 to capacity
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
bool SPIFlash::writeStr(uint32_t _addr, String &inputStr, bool errorCheck) {
  uint8_t _sizeofdata = sizeof(inputStr);
  return _write(_addr, inputStr, _sizeofdata, errorCheck);
}

//Erases one 4k sector. Has two variants:
//	A. Takes the address as the argument and erases the sector containing the address.
//	B. Takes page to be erased as the argument and erases the sector containing the page.
//	The sectors are numbered 0 - 255 containing 16 pages each.
//			Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255
// Variant A
bool SPIFlash::eraseSector(uint32_t _addr) {
  if (!_prep(ERASEFUNC, _addr)) {
    return false;
  }
  _beginSPI(SECTORERASE);   //The address is transferred as a part of this function
  _endSPI();

	if(!_notBusy(500L)) {
    return false;	//Datasheet says erasing a sector takes 400ms max
  }

		//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
		// i.e. to write disable state upon the following conditions:
		// Power-up, Write Disable, Page Program, Quad Page Program, ``Sector Erase``, Block Erase, Chip Erase, Write Status Register,
		// Erase Security Register and Program Security register

	return true;
}

//Erases one 32k block. Has two variants:
//	A. Takes the address as the argument and erases the block containing the address.
//	B. Takes page to be erased as the argument and erases the block containing the page.
//	The blocks are numbered 0 - 31 containing 128 pages each.
// 			Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31
// Variant A
bool SPIFlash::eraseBlock32K(uint32_t _addr) {
  if (!_prep(ERASEFUNC, _addr)) {
    return false;
  }
  _beginSPI(BLOCK32ERASE);
  _endSPI();

	if(!_notBusy(1*S)) {
    return false;	//Datasheet says erasing a sector takes 400ms max
  }

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Erases one 64k block. Has two variants:
//	A. Takes the address as the argument and erases the block containing the address.
//	B. Takes page to be erased as the argument and erases the block containing the page.
//	The blocks are numbered 0 - 15 containing 256 pages each.
// 				Page 0-255 --> Block 0; Page 256-511 --> Block 1;......Page 3840-4095 --> Block 15
//	Variant A
bool SPIFlash::eraseBlock64K(uint32_t _addr) {
  if (!_prep(ERASEFUNC, _addr)) {
    return false;
  }

  _beginSPI(BLOCK64ERASE);
  _endSPI();

	if(!_notBusy(1200L)) {
    return false;	//Datasheet says erasing a sector takes 400ms max
  }

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, ``Block Erase``, Chip Erase, Write Status Register,
	// Erase Security Register and Program Security register

	return true;
}

//Erases whole chip. Think twice before using.
bool SPIFlash::eraseChip(void) {
  //if (!_prep(ERASEFUNC, NULLBYTE)) {
  if(!_notBusy()||!_writeEnable()) {
    return false;
  }

	_beginSPI(CHIPERASE);
  _endSPI();

	while(_readStat1() & BUSY) {
    _delay_us(30000L);
  }

	//_writeDisable(); //_writeDisable() is not required because the Write Enable Latch (WEL) flag is cleared to 0
	// i.e. to write disable state upon the following conditions:
	// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, ``Chip Erase``, Write Status Register,
	// Erase Security Register and Program Security register
  _endSPI();
	return true;

}

//Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase().
//Page Program, Write Status Register, Erase instructions are not allowed.
//Erase suspend is only allowed during Block/Sector erase.
//Program suspend is only allowed during Page/Quad Page Program
bool SPIFlash::suspendProg(void) {
	if(_notBusy()) {
		return false;
  }

  if(!_noSuspend()) {
    return true;
  }

  else {
    _beginSPI(SUSPEND);
    _endSPI();
    _delay_us(20);
    if(!_notBusy(50) || _noSuspend()) {  //Max suspend Enable time according to datasheet
      return false;
    }
    return true;
  }
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {
	if(!_notBusy() || _noSuspend()) {
    return false;
  }

	_beginSPI(RESUME);
	_endSPI();

	_delay_us(20);

	if(_notBusy(10) || !_noSuspend()) {
    return false;
  }
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

  _delay_us(5);

  _beginSPI(WRITEENABLE);
  CHIP_DESELECT
  if (_readStat1() & WRTEN) {
    _endSPI();
    return false;
  }
  else {

    return true;
  }
}

//Wakes chip from low power state.
bool SPIFlash::powerUp(void) {
	_beginSPI(RELEASE);
  _endSPI();
	_delay_us(3);						    //Max release enable time according to the Datasheet

  _beginSPI(WRITEENABLE);
  CHIP_DESELECT
  if (_readStat1() & WRTEN) {
    _endSPI();
    return true;
  }
  else {

    return false;
  }
}
