/* Arduino SPIFlash Library v.2.5.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 13/11/2016
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

#ifndef SPIFLASH_H
#define SPIFLASH_H

#if defined (ARDUINO_ARCH_SAM)
  #include <malloc.h>
  #include <stdlib.h>
  #include <stdio.h>
#endif
#include <Arduino.h>
#ifndef __AVR_ATtiny85__
  #include <SPI.h>
#endif
#include "defines.h"

#if defined (ARDUINO_ARCH_SAM) || defined (ARDUINO_ARCH_SAMD) || defined (ARDUINO_ARCH_ESP8266)
 #define _delay_us(us) delayMicroseconds(us)
#else
 #include <util/delay.h>
#endif

#ifdef ARDUINO_ARCH_AVR
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
    #define CHIP_SELECT   *cs_port &= ~cs_mask;
    #define CHIP_DESELECT *cs_port |=  cs_mask;
    #define xfer(n)   SPI.transfer(n)
  #endif
#elif defined (ARDUINO_ARCH_ESP8266) || defined (ARDUINO_ARCH_SAMD)
    #define CHIP_SELECT   digitalWrite(csPin, LOW);
    #define CHIP_DESELECT digitalWrite(csPin, HIGH);
    #define xfer(n)   SPI.transfer(n)
#elif defined (ARDUINO_ARCH_SAM)
    #define CHIP_SELECT   digitalWrite(csPin, LOW);
    #define CHIP_DESELECT digitalWrite(csPin, HIGH);
    #define xfer   _dueSPITransfer
#endif

#define LIBVER 2
#define LIBSUBVER 5
#define BUGFIXVER 0

#if defined (ARDUINO_ARCH_SAM)
  extern char _end;
  extern "C" char *sbrk(int i);
  //char *ramstart=(char *)0x20070000;
  //char *ramend=(char *)0x20088000;
#endif

class SPIFlash {
public:
  //----------------------------------------------Constructor-----------------------------------------------//
  SPIFlash(uint8_t cs = CS, bool overflow = true);
  //----------------------------------------Initial / Chip Functions----------------------------------------//
  void     begin(void);
  void     setClock(uint32_t clockSpeed);
  bool     libver(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  uint8_t  error(void);
  uint16_t getManID(void);
  uint32_t getJEDECID(void);
  bool     getAddress(uint16_t size, uint16_t &page_number, uint8_t &offset);
  uint32_t getAddress(uint16_t size);
  uint16_t getChipName(void);
  uint16_t sizeofStr(String &inputStr);
  uint32_t getCapacity(void);
  uint32_t getMaxPage(void);
  //-------------------------------------------Write / Read Bytes-------------------------------------------//
  bool     writeByte(uint32_t address, uint8_t data, bool errorCheck = true);
  bool     writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck = true);
  uint8_t  readByte(uint16_t page_number, uint8_t offset, bool fastRead = false);
  uint8_t  readByte(uint32_t address, bool fastRead = false);
  //----------------------------------------Write / Read Byte Arrays----------------------------------------//
  bool     writeByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     writeByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     readByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead = false);
  bool     readByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead = false);
  //-------------------------------------------Write / Read Chars-------------------------------------------//
  bool     writeChar(uint32_t address, int8_t data, bool errorCheck = true);
  bool     writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck = true);
  int8_t   readChar(uint32_t address, bool fastRead = false);
  int8_t   readChar(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //----------------------------------------Write / Read Char Arrays----------------------------------------//
  bool     writeCharArray(uint32_t address, char *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     writeCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     readCharArray(uint32_t address, char *data_buffer, uint16_t buffer_size, bool fastRead = false);
  bool     readCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t buffer_size, bool fastRead = false);
  //------------------------------------------Write / Read Shorts------------------------------------------//
  bool     writeShort(uint32_t address, int16_t data, bool errorCheck = true);
  bool     writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck = true);
  int16_t  readShort(uint32_t address, bool fastRead = false);
  int16_t  readShort(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //-------------------------------------------Write / Read Words-------------------------------------------//
  bool     writeWord(uint32_t address, uint16_t data, bool errorCheck = true);
  bool     writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck = true);
  uint16_t readWord(uint32_t address, bool fastRead = false);
  uint16_t readWord(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //-------------------------------------------Write / Read Longs-------------------------------------------//
  bool     writeLong(uint32_t address, int32_t data, bool errorCheck = true);
  bool     writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck = true);
  int32_t  readLong(uint32_t address, bool fastRead = false);
  int32_t  readLong(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //--------------------------------------Write / Read Unsigned Longs---------------------------------------//
  bool     writeULong(uint32_t address, uint32_t data, bool errorCheck = true);
  bool     writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck = true);
  uint32_t readULong(uint32_t address, bool fastRead = false);
  uint32_t readULong(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //-------------------------------------------Write / Read Floats------------------------------------------//
  bool     writeFloat(uint32_t address, float data, bool errorCheck = true);
  bool     writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck = true);
  float    readFloat(uint32_t address, bool fastRead = false);
  float    readFloat(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //------------------------------------------Write / Read Strings------------------------------------------//
  bool     writeStr(uint32_t address, String &inputStr, bool errorCheck = true);
  bool     writeStr(uint16_t page_number, uint8_t offset, String &inputStr, bool errorCheck = true);
  bool     readStr(uint32_t address, String &outStr, bool fastRead = false);
  bool     readStr(uint16_t page_number, uint8_t offset, String &outStr, bool fastRead = false);
  //------------------------------------------Write / Read Anything-----------------------------------------//
  template <class T> bool writeAnything(uint32_t address, const T& value, bool errorCheck = true);
  template <class T> bool writeAnything(uint16_t page_number, uint8_t offset, const T& value, bool errorCheck = true);
  template <class T> bool readAnything(uint32_t address, T& value, bool fastRead = false);
  template <class T> bool readAnything(uint16_t page_number, uint8_t offset, T& value, bool fastRead = false);
  //--------------------------------------------Erase functions---------------------------------------------//
  bool     eraseSector(uint32_t address);
  bool     eraseSector(uint16_t page_number, uint8_t offset);
  bool     eraseBlock32K(uint32_t address);
  bool     eraseBlock32K(uint16_t page_number, uint8_t offset);
  bool     eraseBlock64K(uint32_t address);
  bool     eraseBlock64K(uint16_t page_number, uint8_t offset);
  bool     eraseChip(void);
  //---------------------------------------------Power functions--------------------------------------------//
  bool     suspendProg(void);
  bool     resumeProg(void);
  bool     powerDown(void);
  bool     powerUp(void);
  //-------------------------------------Public Arduino Due Functions---------------------------------------//
#if defined (ARDUINO_ARCH_SAM)
  uint32_t dueFreeRAM(void);
#endif
  //-------------------------------------------Public variables---------------------------------------------//

private:
#if defined (ARDUINO_ARCH_SAM)
  //-------------------------------------Private Arduino Due Functions--------------------------------------//
  void     _dmac_disable(void);
  void     _dmac_enable(void);
  void     _dmac_channel_disable(uint32_t ul_num);
  void     _dmac_channel_enable(uint32_t ul_num);
  bool     _dmac_channel_transfer_done(uint32_t ul_num);
  void     _dueSPIDmaRX(uint8_t* dst, uint16_t count);
  void     _dueSPIDmaRX(char* dst, uint16_t count);
  void     _dueSPIDmaTX(const uint8_t* src, uint16_t count);
  void     _dueSPIDmaCharTX(const char* src, uint16_t count);
  void     _dueSPIBegin(void);
  void     _dueSPIInit(uint8_t dueSckDivisor);
  uint8_t  _dueSPITransfer(uint8_t b);
  uint8_t  _dueSPIRecByte(void);
  uint8_t  _dueSPIRecByte(uint8_t* buf, size_t len);
  int8_t   _dueSPIRecChar(void);
  int8_t   _dueSPIRecChar(char* buf, size_t len);
  void     _dueSPISendByte(uint8_t b);
  void     _dueSPISendByte(const uint8_t* buf, size_t len);
  void     _dueSPISendChar(char b);
  void     _dueSPISendChar(const char* buf, size_t len);
#endif
  //--------------------------------------------Private functions-------------------------------------------//
  void     _troubleshoot(void);
  void     _cmd(uint8_t cmd, bool _continue = true);
  void     _endProcess(void);
  void     _errorCodeCheck(void);
  void     _endSPI(void);
  bool     _prep(uint8_t opcode, uint32_t address, uint32_t size);
  bool     _prep(uint8_t opcode, uint32_t page_number, uint8_t offset, uint32_t size);
  bool     _startSPIBus(void);
  bool     _beginSPI(uint8_t opcode);
  bool     _noSuspend(void);
  bool     _notBusy(uint32_t timeout = 10L);
  bool     _notPrevWritten(uint32_t address, uint32_t size = 1);
  bool     _writeEnable(uint32_t timeout = 10L);
  bool     _writeDisable(void);
  bool     _getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  bool     _getManId(uint8_t *b1, uint8_t *b2);
  bool     _chipID(void);
  bool     _transferAddress(void);
  bool     _addressCheck(uint32_t address, uint32_t size = 1);
  uint8_t  _nextByte(uint8_t data = NULLBYTE);
  uint8_t  _nextByte(uint8_t opcode, uint8_t data);
  uint16_t _nextInt(uint16_t = NULLINT);
  void     _nextBuf(uint8_t opcode, uint8_t *data_buffer, uint32_t size);
  uint8_t  _readStat1(void);
  uint8_t  _readStat2(void);
  uint32_t _getAddress(uint16_t page_number, uint8_t offset = 0);
  template <class T> bool _writeErrorCheck(uint32_t address, const T& value);
  //-------------------------------------------Private variables------------------------------------------//
  bool        pageOverflow, SPIBusState;
  volatile uint8_t *cs_port;
  uint8_t     cs_mask, csPin, errorcode, state, _SPCR, _SPSR;
  uint16_t    name;
  uint32_t    capacity, maxPage, _eraseTime;
  uint32_t    currentAddress, _currentAddress = 0;
#ifdef SPI_HAS_TRANSACTION
  SPISettings _settings;
#endif
  const uint8_t devType[11]   = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x43};
  const uint32_t memSize[11]  = {64L * 1024L, 128L * 1024L, 256L * 1024L, 512L * 1024L, 1L * 1024L * 1024L,
                                2L * 1024L * 1024L, 4L * 1024L * 1024L, 8L * 1024L * 1024L, 16L * 1024L * 1024L,
                                32L * 1024L * 1024L, 8L * 1024L * 1024L};
  const uint16_t chipName[11] = {05, 10, 20, 40, 80, 16, 32, 64, 128, 256, 64};
  const uint32_t eraseTime[11] = {1L * 1000L, 2L * 1000L, 2L * 1000L, 4L * 1000L, 6L * 1000L, 10 * 1000L, 15 * 1000L, 100 * 1000L, 200 * 1000L, 400 * 1000L, 50L}; //Erase time in milliseconds
};

//--------------------------------------------Templates-------------------------------------------//

// Writes any type of data to a specific location in the flash memory.
// Has two variants:
//  A. Takes two arguments -
//    1. address --> Any address from 0 to maxAddress
//    2. T& value --> Variable to write data from
//    4. errorCheck --> Turned on by default. Checks for writing errors
//  B. Takes three arguments -
//    1. page --> Any page number from 0 to maxPage
//    2. offset --> Any offset within the page - from 0 to 255
//    3. const T& value --> Variable with the data to be written
//    4. errorCheck --> Turned on by default. Checks for writing errors
// WARNING: You can only write to previously erased memory locations (see datasheet).
//      Use the eraseSector()/eraseBlock32K/eraseBlock64K commands to first clear memory (write 0xFFs)
// Variant A
template <class T> bool SPIFlash::writeAnything(uint32_t address, const T& value, bool errorCheck) {
  if (!_prep(PAGEPROG, address, sizeof(value))) {
    return false;
  }
  uint16_t maxBytes = PAGESIZE-(address % PAGESIZE);  // Force the first set of bytes to stay within the first page
  uint16_t length = sizeof(value);

  //if (maxBytes > length) {
    uint32_t writeBufSz;
    uint16_t data_offset = 0;
    const uint8_t* p = ((const uint8_t*)(const void*)&value);

    _startSPIBus();
    while (length > 0)
    {
      writeBufSz = (length<=maxBytes) ? length : maxBytes;

      if(!_notBusy() || !_writeEnable()){
        return false;
      }

      CHIP_SELECT
      (void)xfer(PAGEPROG);
      _transferAddress();

      for (uint16_t i = 0; i < writeBufSz; ++i) {
        _nextByte(*p++);
      }
      _currentAddress += writeBufSz;
      data_offset += writeBufSz;
      length -= writeBufSz;
      maxBytes = 256;   // Now we can do up to 256 bytes per loop
      CHIP_DESELECT
    }
  /*}
  else {
    uint32_t size = sizeof(value);
    _beginSPI(PAGEPROG);
    _nextBuf(PAGEPROG, (uint8_t*)&value, size);
    Serial.print("...");
    CHIP_DESELECT
  }*/

  if (!errorCheck) {
    _endSPI();
    return true;
  }
  else {
    return _writeErrorCheck(address, value);
  }
}
// Variant B
template <class T> bool SPIFlash::writeAnything(uint16_t page_number, uint8_t offset, const T& value, bool errorCheck) {
  uint32_t address = _getAddress(page_number, offset);
  return writeAnything(address, value, errorCheck);
}

// Reads any type of data from a specific location in the flash memory.
// Has two variants:
//  A. Takes two arguments -
//    1. address --> Any address from 0 to maxAddress
//    2. T& value --> Variable to return data into
//    2. fastRead --> defaults to false - executes _beginFastRead() if set to true
//  B. Takes three arguments -
//    1. page --> Any page number from 0 to maxPage
//    2. offset --> Any offset within the page - from 0 to 255
//    3. T& value --> Variable to return data into
//    3. fastRead --> defaults to false - executes _beginFastRead() if set to true
// Variant A
template <class T> bool SPIFlash::readAnything(uint32_t address, T& value, bool fastRead) {
  if (!_prep(READDATA, address, sizeof(value)))
    return false;

    uint8_t* p = (uint8_t*)(void*)&value;
    if(!fastRead)
      _beginSPI(READDATA);
    else
      _beginSPI(FASTREAD);
  for (uint16_t i = 0; i < sizeof(value); i++) {
    *p++ =_nextByte();
  }
  _endSPI();
  return true;
}
// Variant B
template <class T> bool SPIFlash::readAnything(uint16_t page_number, uint8_t offset, T& value, bool fastRead)
{
  uint32_t address = _getAddress(page_number, offset);
  return readAnything(address, value, fastRead);
}

// Private template to check for errors in writing to flash memory
template <class T> bool SPIFlash::_writeErrorCheck(uint32_t address, const T& value) {
if (!_prep(READDATA, address, sizeof(value)) && !_notBusy()) {
  return false;
}

  const uint8_t* p = (const uint8_t*)(const void*)&value;
  _beginSPI(READDATA);
  uint8_t _v;
  for(uint16_t i = 0; i < sizeof(value);i++)
  {
#if defined (ARDUINO_ARCH_SAM)
    if(*p++ != _dueSPIRecByte())
    {
      return false;
    }
#else
    if(*p++ != _nextByte())
    {
      return false;
    }
#endif
  }
  _endSPI();
  return true;
}

#endif // _SPIFLASH_H_
