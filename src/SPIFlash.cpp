/* Arduino SPIFlash Library v.3.1.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 24/02/2018
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
  CHIP_DESELECT
}
#elif defined (ARDUINO_ARCH_SAMD) || defined(ARCH_STM32)
SPIFlash::SPIFlash(uint8_t cs, SPIClass *spiinterface) {
  _spi = spiinterface;  //Sets SPI interface - if no user selection is made, this defaults to SPI
  csPin = cs;
  pinMode(csPin, OUTPUT);
  CHIP_DESELECT
}
#elif defined (BOARD_RTL8195A)
SPIFlash::SPIFlash(PinName cs) {
  gpio_init(&csPin, cs);
  gpio_dir(&csPin, PIN_OUTPUT);
  gpio_mode(&csPin, PullNone);
  gpio_write(&csPin, 1);
  CHIP_DESELECT
}
#else
SPIFlash::SPIFlash(uint8_t cs) {
  csPin = cs;
  pinMode(csPin, OUTPUT);
  CHIP_DESELECT
}
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Public functions used for read, write and erase operations     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Identifies chip and establishes parameters
bool SPIFlash::begin(uint32_t flashChipSize) {
#ifdef PRINTNAMECHANGEALERT
  if (!Serial) {
    Serial.begin(115200);
  }
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print("-");
  }
  Serial.println();
  Serial.println("\t\t\t\t\t\t\t\t\t\tImportant Notice");
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print("-");
  }
  Serial.println();
  Serial.println("\t\t\t\t\tThis version of the library - v3.1.0 will be the last version to ship under the name SPIFlash.");
  Serial.println("\t\t\t\tStarting early May - when v3.2.0 is due - this library will be renamed 'SPIMemory' on the Arduino library manager.");
  Serial.println("\t\t\t\t\t\t\tPlease refer to the Readme file for further details.");
  for (uint8_t i = 0; i < 230; i++) {
    Serial.print("-");
  }
  Serial.println();
#endif
#ifdef RUNDIAGNOSTIC
  Serial.println("Chip Diagnostics initiated.");
  Serial.println();
#endif
#ifdef HIGHSPEED
  Serial.println("Highspeed mode initiated.");
  Serial.println();
#endif
  BEGIN_SPI
#ifdef SPI_HAS_TRANSACTION
  //Define the settings to be used by the SPI bus
  _settings = SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0);
#endif
// If no capacity is defined in user code
  if (!flashChipSize) {
    #ifdef RUNDIAGNOSTIC
    Serial.println("No Chip size defined by user. Automated identification initiated.");
    #endif
    bool retVal = _chipID();
    _endSPI();
    return retVal;
  }
  else {
    _getJedecId();
    // If a custom chip size is defined
    #ifdef RUNDIAGNOSTIC
    Serial.println("Custom Chipsize defined");
    #endif
    _chip.capacity = flashChipSize;
    _chip.supported = false;
  }
  _endSPI();

  if (_chip.manufacturerID == CYPRESS_MANID) {
    setClock(SPI_CLK/4);    // Cypress/Spansion chips appear to perform best at SPI_CLK/4
  }
  chipPoweredDown = false;
  return true;
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
	return (_chip.capacity / SPI_PAGESIZE);
}

//Returns the time taken to run a function. Must be called immediately after a function is run as the variable returned is overwritten each time a function from this library is called. Primarily used in the diagnostics sketch included in the library to track function time.
//This function can only be called if #define RUNDIAGNOSTIC is uncommented in SPIFlash.h
float SPIFlash::functionRunTime(void) {
#ifdef RUNDIAGNOSTIC
  return _spifuncruntime;
#else
  return 0;
#endif
}

//Returns the library version as three bytes
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

// Returns a 64-bit Unique ID that is unique to each flash memory chip
uint64_t SPIFlash::getUniqueID(void) {
  if(!_notBusy() || _isChipPoweredDown()) {
    return false;
   }
  _beginSPI(UNIQUEID);
  for (uint8_t i = 0; i < 4; i++) {
    _nextByte(WRITE, DUMMYBYTE);
  }
  if (address4ByteEnabled) {
    _nextByte(WRITE, DUMMYBYTE);
  }

   for (uint8_t i = 0; i < 8; i++) {
     _uniqueID[i] = _nextByte(READ);
   }
   CHIP_DESELECT

   long long _uid = 0;
   for (uint8_t i = 0; i < 8; i++) {
     _uid += _uniqueID[i];
     _uid = _uid << 8;
   }
   return _uid;
}

//Gets the next available address for use.
// Takes the size of the data as an argument and returns a 32-bit address
// All addresses in the in the sketch must be obtained via this function or not at all.
uint32_t SPIFlash::getAddress(uint16_t size) {
  bool _loopedOver = false;
  if (!_addressCheck(currentAddress, size)){
    return false;
	}
   while (!_notPrevWritten(currentAddress, size)) {
     currentAddress+=size;
    _currentAddress = currentAddress;
    if (_currentAddress >= _chip.capacity) {
      if (_loopedOver) {
        return false;
      }
    #ifdef DISABLEOVERFLOW
      _troubleshoot(OUTOFBOUNDS);
      return false;					// At end of memory - (!pageOverflow)
    #else
      currentAddress = 0x00;// At end of memory - (pageOverflow)
      _loopedOver = true;
    #endif
    }
  }
		uint32_t _addr = currentAddress;
		currentAddress+=size;
		return _addr;
}

//Function for returning the size of the string (only to be used for the getAddress() function)
uint16_t SPIFlash::sizeofStr(String &inputStr) {
  uint16_t size;
  size = (sizeof(char)*(inputStr.length()+1));
  size+=sizeof(inputStr.length()+1);

	return size;
}

// Reads a byte of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
uint8_t SPIFlash::readByte(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  uint8_t data = 0;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads a char of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
int8_t SPIFlash::readChar(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  int8_t data = 0;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads an array of bytes starting from a specific location in a page.
//  Takes four arguments
//    1. _addr --> Any address from 0 to capacity
//    2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//    3. bufferSize --> The size of the buffer - in number of bytes.
//    4. fastRead --> defaults to false - executes _beginFastRead() if set to true

bool  SPIFlash::readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
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
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
	return true;
}

// Reads an array of chars starting from a specific location in a page..
//  Takes four arguments
//    1. _addr --> Any address from 0 to capacity
//    2. data_buffer --> The array of bytes to be read from the flash memory - starting at the address indicated
//    3. bufferSize --> The size of the buffer - in number of bytes.
//    4. fastRead --> defaults to false - executes _beginFastRead() if set to true
bool  SPIFlash::readCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
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
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
	return true;
}

// Reads an unsigned int of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
uint16_t SPIFlash::readWord(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  uint16_t data;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads a signed int of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
int16_t SPIFlash::readShort(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  int16_t data;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads an unsigned long of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
uint32_t SPIFlash::readULong(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  uint32_t data;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads a signed long of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
int32_t SPIFlash::readLong(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  int32_t data;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads a float of data from a specific location in a page.
//  Takes two arguments -
//    1. _addr --> Any address from 0 to capacity
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
float SPIFlash::readFloat(uint32_t _addr, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  float data;
  _read(_addr, data, sizeof(data), fastRead);
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return data;
}

// Reads a string from a specific location on a page.
//  Takes three arguments
//    1. _addr --> Any address from 0 to capacity
//    2. outputString --> String variable to write the output to
//    3. fastRead --> defaults to false - executes _beginFastRead() if set to true
bool SPIFlash::readStr(uint32_t _addr, String &data, bool fastRead) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
    bool _retVal = _read(_addr, data, sizeof(data), fastRead);
    _spifuncruntime = micros() - _spifuncruntime;
    return _retVal;
  #else
  return _read(_addr, data, sizeof(data), fastRead);
  #endif
}

// Writes a byte of data to a specific location in a page.
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One byte to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeByte(uint32_t _addr, uint8_t data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _BYTE_);
}

// Writes a char of data to a specific location in a page.
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One char to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeChar(uint32_t _addr, int8_t data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _CHAR_);
}

// Writes an array of bytes starting from a specific location in a page.
//  Takes four arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data_buffer --> The pointer to the array of bytes be written to a particular location on a page
//    3. bufferSize --> Size of the array of bytes - in number of bytes
//    4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  if (!_prep(PAGEPROG, _addr, bufferSize)) {
    return false;
  }
  uint16_t maxBytes = SPI_PAGESIZE-(_addr % SPI_PAGESIZE);  // Force the first set of bytes to stay within the first page

  if (bufferSize <= maxBytes) {
    CHIP_SELECT
    _nextByte(WRITE, PAGEPROG);
    _transferAddress();
    //_nextBuf(PAGEPROG, &data_buffer[0], bufferSize);
    for (uint16_t i = 0; i < bufferSize; ++i) {
      _nextByte(WRITE, data_buffer[i]);
    }
    CHIP_DESELECT
  }
  else {
    uint16_t length = bufferSize;
    uint16_t writeBufSz;
    uint16_t data_offset = 0;

    do {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;

      CHIP_SELECT
      _nextByte(WRITE, PAGEPROG);
      _transferAddress();
      for (uint16_t i = 0; i < writeBufSz; ++i) {
        _nextByte(WRITE, data_buffer[data_offset + i]);
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
    #ifdef RUNDIAGNOSTIC
      _spifuncruntime = micros() - _spifuncruntime;
    #endif
    return true;
  }
  else {
    if (!_notBusy()) {
      return false;
    }
    _currentAddress = _addr;
    CHIP_SELECT
    _nextByte(WRITE, READDATA);
    _transferAddress();
    for (uint16_t j = 0; j < bufferSize; j++) {
      if (_nextByte(READ) != data_buffer[j]) {
        return false;
      }
    }
    _endSPI();
    #ifdef RUNDIAGNOSTIC
      _spifuncruntime = micros() - _spifuncruntime;
    #endif
    return true;
  }
}

// Writes an array of bytes starting from a specific location in a page.
//  Takes four arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data_buffer --> The pointer to the array of chars be written to a particular location on a page
//    3. bufferSize --> Size of the array of chars - in number of bytes
//    4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool errorCheck) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  if (!_prep(PAGEPROG, _addr, bufferSize)) {
    return false;
  }
  uint16_t maxBytes = SPI_PAGESIZE-(_addr % SPI_PAGESIZE);  // Force the first set of bytes to stay within the first page

  if (bufferSize <= maxBytes) {
    CHIP_SELECT
    _nextByte(WRITE, PAGEPROG);
    _transferAddress();
    //_nextBuf(PAGEPROG, (uint8_t*) &data_buffer[0], bufferSize);
    for (uint16_t i = 0; i < bufferSize; ++i) {
      _nextByte(WRITE, data_buffer[i]);
    }
    CHIP_DESELECT
  }
  else {
    uint16_t length = bufferSize;
    uint16_t writeBufSz;
    uint16_t data_offset = 0;

    do {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;

      CHIP_SELECT
      _nextByte(WRITE, PAGEPROG);
      _transferAddress();
      for (uint16_t i = 0; i < writeBufSz; ++i) {
        _nextByte(WRITE, data_buffer[data_offset + i]);
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
    #ifdef RUNDIAGNOSTIC
      _spifuncruntime = micros() - _spifuncruntime;
    #endif
    return true;
  }
  else {
    if (!_notBusy()) {
      return false;
    }
    _currentAddress = _addr;
    CHIP_SELECT
    _nextByte(WRITE, READDATA);
    _transferAddress();
    for (uint16_t j = 0; j < bufferSize; j++) {
      if (_nextByte(READ) != data_buffer[j]) {
        return false;
      }
    }
    _endSPI();
    #ifdef RUNDIAGNOSTIC
      _spifuncruntime = micros() - _spifuncruntime;
    #endif
    return true;
  }
}

// Writes an unsigned int as two bytes starting from a specific location in a page.
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One word to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeWord(uint32_t _addr, uint16_t data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _WORD_);
}

// Writes a signed int as two bytes starting from a specific location in a page
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One short to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeShort(uint32_t _addr, int16_t data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _SHORT_);
}

// Writes an unsigned long as four bytes starting from a specific location in a page.
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One unsigned long to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeULong(uint32_t _addr, uint32_t data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _ULONG_);
}

// Writes a signed long as four bytes starting from a specific location in a page
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One signed long to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeLong(uint32_t _addr, int32_t data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _LONG_);
}

// Writes a float as four bytes starting from a specific location in a page
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One float to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeFloat(uint32_t _addr, float data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _FLOAT_);
}

// Writes a string to a specific location on a page
//  Takes three arguments -
//    1. _addr --> Any address - from 0 to capacity
//    2. data --> One String to be written to a particular location on a page
//    3. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
// Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
bool SPIFlash::writeStr(uint32_t _addr, String &data, bool errorCheck) {
  return _write(_addr, data, sizeof(data), errorCheck, _STRING_);
}

// Erases a number of sectors or blocks as needed by the data being input.
//  Takes an address and the size of the data being input as the arguments and erases the block/s of memory containing the address.
bool SPIFlash::eraseSection(uint32_t _addr, uint32_t _sz) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif

  if (!_prep(ERASEFUNC, _addr, _sz)) {
    return false;
  }

    // If size of data is > 4KB more than one sector needs to be erased. So the number of erase sessions is determined by the quotient of _sz/KB(4). If the _sz is not perfectly divisible by KB(4), then an additional sector has to be erased.
  uint32_t noOfEraseRunsB4Boundary = 0;
  uint32_t noOf4KBEraseRuns, KB64Blocks, KB32Blocks, KB4Blocks, totalBlocks;

  if (_sz/KB(4)) {
    noOf4KBEraseRuns = _sz/KB(4);
  }
  else {
    noOf4KBEraseRuns = 1;
  }
  KB64Blocks = noOf4KBEraseRuns/16;
  KB32Blocks = (noOf4KBEraseRuns % 16) / 8;
  KB4Blocks = (noOf4KBEraseRuns % 8);
  totalBlocks = KB64Blocks + KB32Blocks + KB4Blocks;
  //Serial.print("noOf4KBEraseRuns: ");
  //Serial.println(noOf4KBEraseRuns);
  //Serial.print("totalBlocks: ");
  //Serial.println(totalBlocks);

  uint16_t _eraseFuncOrder[totalBlocks];

  if (KB64Blocks) {
    for (uint32_t i = 0; i < KB64Blocks; i++) {
      _eraseFuncOrder[i] = BLOCK64ERASE;
    }
  }
  if (KB32Blocks) {
    for (uint32_t i = KB64Blocks; i < (KB64Blocks + KB32Blocks); i++) {
      _eraseFuncOrder[i] = BLOCK32ERASE;
    }
  }
  if (KB4Blocks) {
    for (uint32_t i = (KB64Blocks + KB32Blocks); i < totalBlocks; i++) {
      _eraseFuncOrder[i] = SECTORERASE;
    }
  }

// Now that the number of blocks to be erased have been calculated and the information saved, the erase function is carried out.
  if (_addressOverflow) {
    noOfEraseRunsB4Boundary = (_sz - _addressOverflow)/16;
    noOfEraseRunsB4Boundary += ((_sz - _addressOverflow) % 16) / 8;
    noOfEraseRunsB4Boundary += ((_sz - _addressOverflow) % 8);
    //Serial.print("noOfEraseRunsB4Boundary: ");
    //Serial.println(noOfEraseRunsB4Boundary);
  }
  if (!_addressOverflow) {
    for (uint32_t j = 0; j < totalBlocks; j++) {
      _beginSPI(_eraseFuncOrder[j]);   //The address is transferred as a part of this function
      _endSPI();


      //Serial.print("_eraseFuncOrder: 0x");
      //Serial.println(_eraseFuncOrder[j], HEX);

      uint16_t _timeFactor = 0;
      switch (_eraseFuncOrder[j]) {
        case BLOCK64ERASE:
        _timeFactor = 1200;
        break;

        case BLOCK32ERASE:
        _timeFactor = 1000;
        break;

        case SECTORERASE:
        _timeFactor = 500;
        break;

      }
      if(!_notBusy(_timeFactor * 1000L)) {
        return false;	//Datasheet says erasing a sector takes 400ms max
      }
      if (j == noOfEraseRunsB4Boundary) {
        if (!_prep(ERASEFUNC, (_addr + (_sz - _addressOverflow)), _sz)) {
          return false;
        }
        //Serial.print("Overflow triggered");
      }
    }
  }
  //_writeDisable();
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif

	return true;
}

// Erases one 4k sector.
//  Takes an address as the argument and erases the block containing the address.
bool SPIFlash::eraseSector(uint32_t _addr) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  if (!_prep(ERASEFUNC, _addr, KB(4))) {
    return false;
  }
  _beginSPI(SECTORERASE);   //The address is transferred as a part of this function
  _endSPI();

  if(!_notBusy(500 * 1000L)) {
    return false;	//Datasheet says erasing a sector takes 400ms max
  }
  //_writeDisable();
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif

	return true;
}

// Erases one 32k block.
//  Takes an address as the argument and erases the block containing the address.
bool SPIFlash::eraseBlock32K(uint32_t _addr) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  if (!_prep(ERASEFUNC, _addr, KB(32))) {
    return false;
  }
  _beginSPI(BLOCK32ERASE);
  _endSPI();

  if(!_notBusy(1000 * 1000L)) {
    return false;	//Datasheet says erasing a sector takes 400ms max
  }
  _writeDisable();
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif

	return true;
}

// Erases one 64k block.
//  Takes an address as the argument and erases the block containing the address.
bool SPIFlash::eraseBlock64K(uint32_t _addr) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
  if (!_prep(ERASEFUNC, _addr, KB(64))) {
    return false;
  }

  _beginSPI(BLOCK64ERASE);
  _endSPI();

  if(!_notBusy(1200 * 1000L)) {
    return false;	//Datasheet says erasing a sector takes 400ms max
  }
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
	return true;
}

//Erases whole chip. Think twice before using.
bool SPIFlash::eraseChip(void) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
	if(_isChipPoweredDown() || !_notBusy() || !_writeEnable()) {
    return false;
  }

	_beginSPI(CHIPERASE);
  _endSPI();

	while(_readStat1() & BUSY) {
    //_delay_us(30000L);
  }
  _endSPI();

  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
	return true;

}

//Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase().
//Page Program, Write Status Register, Erase instructions are not allowed.
//Erase suspend is only allowed during Block/Sector erase.
//Program suspend is only allowed during Page/Quad Page Program
bool SPIFlash::suspendProg(void) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
	if(_isChipPoweredDown() || _notBusy()) {
		return false;
  }

  if(!_noSuspend()) {
    #ifdef RUNDIAGNOSTIC
      _spifuncruntime = micros() - _spifuncruntime;
    #endif
    return true;
  }

  _beginSPI(SUSPEND);
  _endSPI();
  _delay_us(20);
  if(!_notBusy(50) || _noSuspend()) {
    return false;
  }
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return true;
}

//Resumes previously suspended Block Erase/Sector Erase/Page Program.
bool SPIFlash::resumeProg(void) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
	if(_isChipPoweredDown() || !_notBusy() || _noSuspend()) {
    return false;
  }

	_beginSPI(RESUME);
	_endSPI();

	_delay_us(20);

	if(_notBusy(10) || !_noSuspend()) {
    return false;
  }
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros() - _spifuncruntime;
  #endif
  return true;

}

//Puts device in low power state. Good for battery powered operations.
//In powerDown() the chip will only respond to powerUp()
bool SPIFlash::powerDown(void) {
  if (_chip.manufacturerID != MICROCHIP_MANID) {
    #ifdef RUNDIAGNOSTIC
      _spifuncruntime = micros();
    #endif
  	if(!_notBusy(20))
  		return false;

  	_beginSPI(POWERDOWN);
    _endSPI();

    _delay_us(5);

    #ifdef RUNDIAGNOSTIC
      chipPoweredDown = true;
      bool _retVal = !_writeEnable(false);
      _spifuncruntime = micros() - _spifuncruntime;
      return _retVal;
    #else
      chipPoweredDown = true;
      return !_writeEnable(false);
    #endif
  }
  else {
    _troubleshoot(UNSUPPORTEDFUNC);
    return false;
  }
}

//Wakes chip from low power state.
bool SPIFlash::powerUp(void) {
  #ifdef RUNDIAGNOSTIC
    _spifuncruntime = micros();
  #endif
	_beginSPI(RELEASE);
  _endSPI();
	_delay_us(3);						    //Max release enable time according to the Datasheet

  #ifdef RUNDIAGNOSTIC
    if (_writeEnable(false)) {
      _writeDisable();
      _spifuncruntime = micros() - _spifuncruntime;
      chipPoweredDown = false;
      return true;
    }
    return false;
  #else
  if (_writeEnable(false)) {
    _writeDisable();
    chipPoweredDown = false;
    return true;
  }
  return false;
  #endif
}

/* Note: _writeDisable() is not required at the end of any function that writes to the Flash memory because the Write Enable Latch (WEL) flag is cleared to 0 i.e. to write disable state upon the following conditions being completed:
Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register, Erase Security Register and Program Security register */
