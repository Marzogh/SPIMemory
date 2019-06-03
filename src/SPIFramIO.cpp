/* Arduino SPIMemory Library v.3.4.0
 * Copyright (C) 2019 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/06/2018
 *
 * This file is part of the Arduino SPIMemory Library. This library is for
 * Flash and FRAM memory modules. In its current form it enables reading,
 * writing and erasing data from and to various locations;
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
 * along with the Arduino SPIMemory Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

 #include "SPIFram.h"

 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //     Private functions used by read, write and erase operations     //
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 // Creates bit mask from bit x to bit y
 unsigned SPIFram::_createMask(unsigned x, unsigned y) {
   unsigned r = 0;
   for (unsigned i=x; i<=y; i++) {
     r |= 1 << i;
   }
   return r;
 }

 //Checks to see if page overflow is permitted and assists with determining next address to read/write.
 //Sets the global address variable
 bool SPIFram::_addressCheck(uint32_t _addr, uint32_t size) {
   uint32_t _submittedAddress = _addr;
   uint8_t _errorcode = error();
   if (_errorcode == UNKNOWNCAP || _errorcode == NORESPONSE) {
     return false;
   }
 	if (!_chip.capacity) {
     _troubleshoot(CALLBEGIN);
     return false;
 	}

   //Serial.print("_chip.capacity: ");
   //Serial.println(_chip.capacity, HEX);

   if (_submittedAddress + size >= _chip.capacity) {
     //Serial.print("_submittedAddress + size: ");
     //Serial.println(_submittedAddress + size, HEX);
   #ifdef DISABLEOVERFLOW
     _troubleshoot(OUTOFBOUNDS);
     return false;					// At end of memory - (!pageOverflow)
   #else
     _addressOverflow = ((_submittedAddress + size) - _chip.capacity);
     _currentAddress = _addr;
     //Serial.print("_addressOverflow: ");
     //Serial.println(_addressOverflow, HEX);
     return true;					// At end of memory - (pageOverflow)
   #endif
   }
   else {
     _addressOverflow = false;
     _currentAddress = _addr;
     return true;				// Not at end of memory if (address < _chip.capacity)
   }
   //Serial.print("_currentAddress: ");
   //Serial.println(_currentAddress, HEX);
 }

 // Checks to see if the block of memory has been previously written to (unless OVERWRITE is enabled)
 bool SPIFram::_notPrevWritten(uint32_t _addr, uint32_t size) {
  #if !defined (OVERWRITE)
   _beginSPI(READDATA);
   for (uint32_t i = 0; i < size; i++) {if (_nextByte(READ) != 0x00) {
       CHIP_DESELECT;
       _troubleshoot(PREVWRITTEN);
       return false;
     }
   }
   CHIP_DESELECT
  #endif
   return true;
 }

 //Double checks all parameters before calling a read or write. Comes in two variants
 //Takes address and returns the address if true, else returns false. Throws an error if there is a problem.
 bool SPIFram::_prep(uint32_t _addr, uint32_t size) {
   if(_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notPrevWritten(_addr, size) || !_writeEnable()) {
     return false;
   }
   return true;
 }

 // Transfer Address.
 bool SPIFram::_transferAddress(void) {
   _nextByte(WRITE, Hi(_currentAddress));
   _nextByte(WRITE, Lo(_currentAddress));
   return true;
 }

 bool SPIFram::_startSPIBus(void) {
   #ifndef SPI_HAS_TRANSACTION
       noInterrupts();
   #endif

   #if defined (ARDUINO_ARCH_SAM)
     due.SPIInit(DUE_SPI_CLK);
   #elif defined (ARDUINO_ARCH_SAMD)
     #ifdef SPI_HAS_TRANSACTION
       _spi->beginTransaction(_settings);
     #else
       _spi->setClockDivider(SPI_CLOCK_DIV_4);
       _spi->setDataMode(SPI_MODE0);
       _spi->setBitOrder(MSBFIRST);
     #endif
     #if defined ENABLEZERODMA
       dma_init();
     #endif
   #else
     #if defined (ARDUINO_ARCH_AVR)
       //save current SPI settings
       _SPCR = SPCR;
       _SPSR = SPSR;
     #endif
     #ifdef SPI_HAS_TRANSACTION
       SPI.beginTransaction(_settings);
     #else
       SPI.setClockDivider(SPI_CLOCK_DIV4);
       SPI.setDataMode(SPI_MODE0);
       SPI.setBitOrder(MSBFIRST);
     #endif
   #endif
   SPIBusState = true;
   return true;
 }

 //Initiates SPI operation - but data is not transferred yet. Always call _prep() before this function (especially when it involves writing or reading to/from an address)
 bool SPIFram::_beginSPI(uint8_t opcode) {
   if (!SPIBusState) {
     _startSPIBus();
   }
   CHIP_SELECT
   switch (opcode) {
     case READDATA:
     _nextByte(WRITE, opcode);
     _transferAddress();
     break;

     case PAGEPROG:
     _nextByte(WRITE, opcode);
     _transferAddress();
     break;

     case FASTREAD:
     _nextByte(WRITE, opcode);
     _nextByte(WRITE, DUMMYBYTE);
     _transferAddress();
     break;

     default:
     _nextByte(WRITE, opcode);
     break;
   }
   return true;
 }
 //SPI data lines are left open until _endSPI() is called

 //Reads/Writes next byte. Call 'n' times to read/write 'n' number of bytes. Should be called after _beginSPI()
 uint8_t SPIFram::_nextByte(char IOType, uint8_t data) {
 #if defined (ARDUINO_ARCH_SAMD)
   #ifdef ENABLEZERODMA
     union {
       uint8_t dataBuf[1];
       uint8_t val;
     } rxData, txData;
     txData.val = data;
     spi_transfer(txData.dataBuf, rxData.dataBuf, 1);
     return rxData.val;
   #else
     return xfer(data);
   #endif
 #else
   return xfer(data);
 #endif
 }

 //Reads/Writes next int. Call 'n' times to read/write 'n' number of integers. Should be called after _beginSPI()
 uint16_t SPIFram::_nextInt(uint16_t data) {
 #if defined (ARDUINO_ARCH_SAMD)
   return _spi->transfer16(data);
 #else
   return SPI.transfer16(data);
 #endif
 }

 //Reads/Writes next data buffer. Should be called after _beginSPI()
 void SPIFram::_nextBuf(uint8_t opcode, uint8_t *data_buffer, uint32_t size) {
   uint8_t *_dataAddr = &(*data_buffer);
   switch (opcode) {
     case READDATA:
     #if defined (ARDUINO_ARCH_SAM)
       due.SPIRecByte(&(*data_buffer), size);
     #elif defined (ARDUINO_ARCH_SAMD)
       #ifdef ENABLEZERODMA
         spi_read(&(*data_buffer), size);
       #else
         _spi->transfer(&data_buffer[0], size);
       #endif
     #elif defined (ARDUINO_ARCH_AVR)
       SPI.transfer(&(*data_buffer), size);
     #else
       for (uint16_t i = 0; i < size; i++) {
         *_dataAddr = xfer(NULLBYTE);
         _dataAddr++;
       }
     #endif
     break;

     case PAGEPROG:
     #if defined (ARDUINO_ARCH_SAM)
       due.SPISendByte(&(*data_buffer), size);
     #elif defined (ARDUINO_ARCH_SAMD)
       #ifdef ENABLEZERODMA
         spi_write(&(*data_buffer), size);
       #else
         _spi->transfer(&data_buffer[0], size);
       #endif
     #elif defined (ARDUINO_ARCH_AVR)
       SPI.transfer(&(*data_buffer), size);
     #else
       for (uint16_t i = 0; i < size; i++) {
         xfer(*_dataAddr);
         _dataAddr++;
       }
     #endif
     break;
   }
 }

 //Stops all operations. Should be called after all the required data is read/written from repeated _nextByte() calls
 void SPIFram::_endSPI(void) {
   CHIP_DESELECT

 #ifdef SPI_HAS_TRANSACTION
   #if defined (ARDUINO_ARCH_SAMD)
     _spi->endTransaction();
   #else
     SPI.endTransaction();
   #endif
 #else
   interrupts();
 #endif
 #if defined (ARDUINO_ARCH_AVR)
   SPCR = _SPCR;
   SPSR = _SPSR;
 #endif
   SPIBusState = false;
 }

 // Checks if status register 1 can be accessed - used to check chip status, during powerdown and power up and for debugging
 uint8_t SPIFram::_readStat1(void) {
   _beginSPI(READSTAT1);
   stat1 = _nextByte(READ);
   CHIP_DESELECT
   return stat1;
 }

 // Checks to see if chip is powered down. If it is, retrns true. If not, returns false.
 bool SPIFram::_isChipPoweredDown(void) {
   if (chipPoweredDown) {
     _troubleshoot(CHIPISPOWEREDDOWN);
     return true;
   }
   else {
     return false;
   }
 }

 //Enables writing to chip by setting the WRITEENABLE bit
 bool SPIFram::_writeEnable(bool _troubleshootEnable) {
   _beginSPI(WRITEENABLE);
   CHIP_DESELECT
   if (!(_readStat1() & WRTEN)) {
     if (_troubleshootEnable) {
       _troubleshoot(CANTENWRITE);
     }
     return false;
   }
   return true;
 }

 //Disables writing to chip by setting the Write Enable Latch (WEL) bit in the Status Register to 0
 //_writeDisable() is not required under the following conditions because the Write Enable Latch (WEL) flag is cleared to 0
 // i.e. to write disable state:
 // Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register,
 // Erase Security Register and Program Security register
 bool SPIFram::_writeDisable(void) {
 	_beginSPI(WRITEDISABLE);
   CHIP_DESELECT
 	return true;
}

 //Checks for presence of chip by requesting JEDEC ID
 bool SPIFram::_getJedecId(void) {
   _beginSPI(JEDECID);
   do {
     _chip.manufacturerID = _nextByte(READ);		// manufacturer id
   } while (_chip.manufacturerID == 0x7F);    // 0x7F is a continuation code according to JEDEC. So, the code loops till a manufacturer ID is read.
 	_chip.devID1 = _nextByte(READ);		// memory type
 	_chip.devID2 = _nextByte(READ);		// capacity
   CHIP_DESELECT
   if (_chip.manufacturerID == 0x00) {
     _troubleshoot(NORESPONSE);
     return false;
   }
   else {
     return true;
   }
 }

 bool SPIFram::_disableGlobalBlockProtect(void) {
   // TODO: write function for _disableGlobalBlockProtect()
   return true;
 }

 //Identifies the chip
 bool SPIFram::_chipID(uint32_t flashChipSize) {

   if (!_chip.capacity) {
     if (flashChipSize) {
       // If a custom chip size is defined
       #ifdef RUNDIAGNOSTIC
       Serial.println("Custom Chipsize defined");
       #endif
       _chip.capacity = flashChipSize;
       _chip.supported = false;
       return true;
     }

     else {
       _getJedecId();
       for (uint8_t i = 0; i < sizeof(_supportedManID); i++) {
         if (_chip.manufacturerID == _supportedManID[i]) {
           _chip.supportedMan = true;
           break;
         }
       }
       if (_chip.supportedMan) {
         #ifdef RUNDIAGNOSTIC
           Serial.println("No Chip size defined by user. Checking library support.");
         #endif
         //Identify capacity
         _chip.capacity = ((_chip.devID1 & _createMask(0, 4)) * KB(16)); // Currently only tested to be compatible with the FM25 series FRAM from Cypress. Refer to datasheet for FM25V05, Page 9
       }
       else {
         _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
         return false;
       }
     }
     _troubleshoot(UNKNOWNCAP);
     return false;
   }
   return true;
 }

 //Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of this file.
 void SPIFram::_troubleshoot(uint8_t _code, bool printoverride) {
   diagnostics.troubleshoot(_code, printoverride);
 }
