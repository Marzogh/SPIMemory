/* Arduino SPIMemory Library v.3.4.0
 * Copyright (C) 2019 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 03/06/2019
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

#include "SPIFlash.h"

/**************************************************************
* Private functions used by read, write and erase operations *
**************************************************************/

/**
 * @brief   Creates bit mask from bit x to bit y
 *
 * @param   x   Lowest digit to mask
 * @param   y   Highest digit to mask
 * @returns Bit-masked binary of user-defined size
 * @exceptsafe  **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see
 */
unsigned SPIFlash::_createMask(unsigned x, unsigned y)
{
    unsigned r = 0;

    for (unsigned i = x; i <= y; i++) {
        r |= 1 << i;
    }
    return r;
}

/**
 * @brief   Sets default parameters for flash chip's SPI functions
 *
 * @exceptsafe  **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see     _chipID()
 */
void SPIFlash::_setDefaultParams(void)
{
    // set some default values
    kb4Erase.supported   = kb32Erase.supported = kb64Erase.supported = chipErase.supported = true;
    kb4Erase.opcode      = SECTORERASE;
    kb32Erase.opcode     = BLOCK32ERASE;
    kb64Erase.opcode     = BLOCK64ERASE;
    kb4Erase.time        = BUSY_TIMEOUT;
    kb32Erase.time       = kb4Erase.time * 8;
    kb64Erase.time       = kb32Erase.time * 4;
    kb256Erase.supported = false;
    chipErase.opcode     = CHIPERASE;
    chipErase.time       = kb64Erase.time * 100L;
    _pageSize = SPI_PAGESIZE;
}

/**
 * @brief   Checks a memory block starting at a specific address for issues
 * Checks user input address to:
 *    1. Check if the address is valid
 *    2. Check if page overflow is permitted
 *    3. Assist with determining next address to read/write
 *    4. Set the global address variable
 *
 * @param      _addr   Start address of the memory block to be checked
 * @param      size    Size of data to be written to memory (in bytes)
 * @returns    TRUE indicates if the memory block can be successfully written to. FALSE indicates otherwise.
 * @throws  `CALLBEGIN` if chip's capacity cannot be identified.
 * @throws  `OUTOFBOUNDS` if `#define DISABLEOVERFLOW` has been uncommented and the address is outside the capacity of the flash chip.
 *
 * @exceptsafe  **basic** If the function throws an exception, the program will be in a valid state, but not necessarily a predictable one. No memory, file descriptors, locks, or other resources will be leaked.
 * @see     begin
 */
bool SPIFlash::_addressCheck(uint32_t _addr, uint32_t size)
{
    uint32_t _submittedAddress = _addr;
    uint8_t _errorcode         = error();

    if (_errorcode == UNKNOWNCAP || _errorcode == NORESPONSE) {
        return false;
    }
    if (!_chip.capacity) {
        _troubleshoot(CALLBEGIN);
        return false;
    }

    // Serial.print(F("_chip.capacity: "));
    // Serial.println(_chip.capacity, HEX);

    if (_submittedAddress + size >= _chip.capacity) {
        // Serial.print(F("_submittedAddress + size: "));
        // Serial.println(_submittedAddress + size, HEX);
        #ifdef DISABLEOVERFLOW
        _troubleshoot(OUTOFBOUNDS);
        return false; // At end of memory - (!pageOverflow)

        #else
        _addressOverflow = ((_submittedAddress + size) - _chip.capacity);
        _currentAddress  = _addr;
        // Serial.print(F("_addressOverflow: "));
        // Serial.println(_addressOverflow, HEX);
        return true; // At end of memory - (pageOverflow)

        #endif
    } else {
        _addressOverflow = false;
        _currentAddress  = _addr;
        return true; // Not at end of memory if (address < _chip.capacity)
    }
    // Serial.print(F("_currentAddress: "));
    // Serial.println(_currentAddress, HEX);
} // SPIFlash::_addressCheck

/**
 * @brief   Checks to see if a block of memory has been previously written to
 * Checks to see if the memory block passed to it contains previously written data.
 *
 * @param      _addr   Address where memory block begins
 * @param      size    Size of data to be written to memory (in bytes)
 * @returns    TRUE if block of memory has **no** previously written data in it. FALSE if block of memory has previously written data in it
 * @throws     `PREVWRITTEN` if the memory block has data in it
 * @exceptsafe  **basic** If the function throws an exception, the program will be in a valid state, but not necessarily a predictable one. No memory, file descriptors, locks, or other resources will be leaked.
 */
// TODO Allow this function to be called publicly as well.
bool SPIFlash::_notPrevWritten(uint32_t _addr, uint32_t size)
{
    _beginSPI(READDATA);
    for (uint32_t i = 0; i < size; i++) {
        if (_nextByte() != 0xFF) {
            CHIP_DESELECT;
            _troubleshoot(PREVWRITTEN);
            return false;
        }
    }
    CHIP_DESELECT
    return true;
} // SPIFlash::_notPrevWritten

/**
 * @brief   Double checks all required chip conditions before calling a read or write.
 * Checks the chips for the following conditions Must be called before any instruction is passed to the chip.
 *        1. Enables 4 byte addressing if required (Winbond only for now)
 *        2. Checks to see if the chip is **not** powered down
 *        3. Checks to see if the memory block to be written to is available
 *        4. Checks to see if the memory block to be written to has not been previously written to
 *        5. Checks to see if the chip is not busy with another operation
 *        6. Sets the `WRITEENABLE` register in the chip's status register to enable write operations
 *
 * @param      opcode   Indicates upcoming function call to allow for function-specific checks to be made.
 * @param      _addr    Address where memory block begins
 * @param      size     Size of data to be written to/read from memory (in bytes)
 * @returns    TRUE if the chip meets all the conditions. FALSE if it doesn't.
 * @throws     Any errors thown here are specific to the internal functions called.
 * @exceptsafe **basic** If the function throws an exception, the program will be in a valid state, but not necessarily a predictable one. No memory, file descriptors, locks, or other resources will be leaked.
 */
bool SPIFlash::_prep(uint8_t opcode, uint32_t _addr, uint32_t size)
{
    // If the flash memory is >= 256 MB enable 4-byte addressing
    if (_chip.manufacturerID == WINBOND_MANID && _addr >= MB(16)) {
        if (!_enable4ByteAddressing()) { // If unable to enable 4-byte addressing
            return false;
        } // TODO: Add SFDP compatibility here
    }
    switch (opcode) {
        case PAGEPROG:
            // Serial.print(F("Address being prepped: "));
            // Serial.println(_addr);
            #ifndef HIGHSPEED
            if (_isChipPoweredDown() ||
              !_addressCheck(_addr, size) || !_notPrevWritten(_addr, size) || !_notBusy() || !_writeEnable())
            {
                return false;
            }
            #else
            if (_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notBusy() || !_writeEnable()) {
                return false;
            }
            #endif
            return true;

            break;

        case ERASEFUNC:
            if (_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notBusy() || !_writeEnable()) {
                return false;
            }
            return true;

            break;

        default:
            if (_isChipPoweredDown() || !_addressCheck(_addr, size) || !_notBusy()) {
                return false;
            }
            #ifdef ENABLEZERODMA
            _delay_us(3500L);
            #endif
            return true;

            break;
    }
} // SPIFlash::_prep

// Transfer Address.

/**
 * @brief      Transfer currently active address to the chip
 *
 * @returns    TRUE if transfer is successful. FALSE if unsuccessful.
 * @exceptsafe **basic** If the function throws an exception, the program will be in a valid state, but not necessarily a predictable one. No memory, file descriptors, locks, or other resources will be leaked.
 */
bool SPIFlash::_transferAddress(void)
{
    if (address4ByteEnabled) {
        _nextByte(Highest(_currentAddress));
    }
    _nextByte(Higher(_currentAddress));
    _nextByte(Hi(_currentAddress));
    _nextByte(Lo(_currentAddress));
    return true;
}

/**
 * @brief      Starts up the SPI bus
 * Checks if SPI Transactions are enabled. Sets up SPI Clock divider, SPI data mode, SPI Bit order; initializes DMA mode (optional) and sets the global `SPIBusState` variable to TRUE to prevent re-running all these steps when the SPI bus is active.
 *
 * @returns    TRUE
 * @exceptsafe **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see
 */
bool SPIFlash::_startSPIBus(void)
{
    #ifndef SPI_HAS_TRANSACTION
    noInterrupts();
    #endif

    #if defined(ARDUINO_ARCH_SAM)
    due.SPIInit(DUE_SPI_CLK);
    #elif defined(ARDUINO_ARCH_SAMD)
    # ifdef SPI_HAS_TRANSACTION
    _spi->beginTransaction(_settings);
    # else
    _spi->setClockDivider(SPI_CLOCK_DIV_4);
    _spi->setDataMode(SPI_MODE0);
    _spi->setBitOrder(MSBFIRST);
    # endif
    # if defined ENABLEZERODMA
    dma_init();
    # endif
    #else // if defined(ARDUINO_ARCH_SAM)
    # if defined(ARDUINO_ARCH_AVR)
    // save current SPI settings
    _SPCR = SPCR;
    _SPSR = SPSR;
    # endif
    # ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(_settings);
    # else
    SPI.setClockDivider(_clockdiv);
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    # endif
    #endif // if defined(ARDUINO_ARCH_SAM)
    SPIBusState = true;
    return true;
} // SPIFlash::_startSPIBus

/**
 * @brief      Begins an SPI comms operation
 * Configures and starts up the SPIBus if it has not already been instantiated. Transfers the opcode, optionally followed by the memory address, for the I/O operation, to the chip.
 *
 * @param      opcode     The command to be sent to the chip.
 * @returns    TRUE
 * @exceptsafe **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see       _nextByte
 * @see       _nextBuf
 * @note      No data is transferred to the flash chip with this function. Always call _prep() before this function (especially when it involves writing or reading to/from an address). SPI data lines are left open until _endSPI() is called.
 */
bool SPIFlash::_beginSPI(uint8_t opcode)
{
    if (!SPIBusState) {
        _startSPIBus();
    }
    CHIP_SELECT
    switch (opcode) {
        case READDATA:
            _nextByte(opcode);
            _transferAddress();
            break;

        case PAGEPROG:
            _nextByte(opcode);
            _transferAddress();
            break;

        case FASTREAD:
            _nextByte(opcode);
            _transferAddress();
            _nextByte(DUMMYBYTE);
            break;
        // TODO Fix erase opcodes - they're not always the same! For eg. force the opcode transferred here to be the kb64Erase.opcode and force _beginSPI in sector erase to transfer SECTORERASE only.
        case SECTORERASE:
            _nextByte(opcode);
            _transferAddress();
            break;

        case BLOCK32ERASE:
            _nextByte(opcode);
            _transferAddress();
            break;

        case BLOCK64ERASE:
            _nextByte(opcode);
            _transferAddress();
            break;
        // UNIQUEID, CHIPERASE, ALT_CHIPERASE, SUSPEND, RESUME, POWERDOWN, RELEASE, JEDECID, MANID, READSFDP
        default:
            _nextByte(opcode);
            break;
    }
    return true;
} // SPIFlash::_beginSPI

/**
 * @brief      Reads/Writes next byte.
 * Reads/Writes next byte of data to/from the chip. Should be called after _beginSPI()
 *
 * @param      data Data to be written to / read from the chip
 * @returns    One byte (unsigned 8-bit)
 * @throws
 * @exceptsafe **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see       _beginSPI
 * @see       _endSPI
 * @note      Should be called after _beginSPI()
 */

uint8_t SPIFlash::_nextByte(uint8_t data)
{
    #if defined(ARDUINO_ARCH_SAMD)
    # ifdef ENABLEZERODMA
    union {
        uint8_t dataBuf[1];
        uint8_t val;
    } rxData, txData;
    txData.val = data;
    spi_transfer(txData.dataBuf, rxData.dataBuf, 1);
    return rxData.val;

    # else
    return xfer(data);

    # endif // ifdef ENABLEZERODMA
    #else // if defined(ARDUINO_ARCH_SAMD)
    return xfer(data);

    #endif // if defined(ARDUINO_ARCH_SAMD)
}

/**
 * @brief      Reads/Writes next int.
 * Reads/Writes next int of data to/from the chip. Should be called after _beginSPI()
 *
 * @param      data Data to be written to / read from the chip
 * @returns    One int (unsigned 16-bit)
 * @throws
 * @exceptsafe **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see       _beginSPI
 * @see       _endSPI
 * @note      Should be called after _beginSPI()
 */
uint16_t SPIFlash::_nextInt(uint16_t data)
{
    #if defined(ARDUINO_ARCH_SAMD)
    return _spi->transfer16(data);

    #else
    return SPI.transfer16(data);

    #endif
}

/**
 * @brief      Reads/Writes next array of bytes.
 * Reads/Writes next array of bytes of data to/from the chip. Should be called after _beginSPI()
 *
 * @param      opcode         Indicates if the command is to read or write
 * @param      data_buffer    Data to be written to / read from the chip in the form of am array of bytes.
 * @param      size           Number of members in the array
 * @returns    One array of bytes (unsigned 8-bit) with user-defined number of members
 * @exceptsafe **no-throw** The function is guaranteed to always return without throwing an exception.
 * @see       _beginSPI
 * @see       _endSPI
 * @note      Should be called after _beginSPI()
 */

// Reads/Writes next data buffer. Should be called after _beginSPI()
void SPIFlash::_nextBuf(uint8_t opcode, uint8_t * data_buffer, uint32_t size)
{
    #if !defined(ARDUINO_ARCH_SAM) && !defined(ARDUINO_ARCH_SAMD) && !defined(ARDUINO_ARCH_AVR)
    uint8_t * _dataAddr = &(*data_buffer);
    #endif

    switch (opcode) {
        case READDATA:
            #if defined(ARDUINO_ARCH_SAM)
            due.SPIRecByte(&(*data_buffer), size);
            #elif defined(ARDUINO_ARCH_SAMD)
            # ifdef ENABLEZERODMA
            spi_read(&(*data_buffer), size);
            # else
            _spi->transfer(&data_buffer[0], size);
            # endif
            #elif defined(ARDUINO_ARCH_AVR)
            SPI.transfer(&(*data_buffer), size);
            #else
            for (uint16_t i = 0; i < size; i++) {
                *_dataAddr = xfer(NULLBYTE);
                _dataAddr++;
            }
            #endif // if defined(ARDUINO_ARCH_SAM)
            break;

        case PAGEPROG:
            #if defined(ARDUINO_ARCH_SAM)
            due.SPISendByte(&(*data_buffer), size);
            #elif defined(ARDUINO_ARCH_SAMD)
            # ifdef ENABLEZERODMA
            spi_write(&(*data_buffer), size);
            # else
            _spi->transfer(&data_buffer[0], size);
            # endif
            #elif defined(ARDUINO_ARCH_AVR)
            SPI.transfer(&(*data_buffer), size);
            #else
            for (uint16_t i = 0; i < size; i++) {
                xfer(*_dataAddr);
                _dataAddr++;
            }
            #endif // if defined(ARDUINO_ARCH_SAM)
            break;
    }
} // SPIFlash::_nextBuf

// Stops all operations. Should be called after all the required data is read/written from repeated _nextByte() calls
void SPIFlash::_endSPI(void)
{
    CHIP_DESELECT

    if (address4ByteEnabled) { // If the previous operation enabled 4-byte addressing, disable it
        _disable4ByteAddressing();
    }

    #ifdef SPI_HAS_TRANSACTION
    # if defined(ARDUINO_ARCH_SAMD)
    _spi->endTransaction();
    # else
    SPI.endTransaction();
    # endif
    #else
    interrupts();
    #endif
    #if defined(ARDUINO_ARCH_AVR)
    SPCR = _SPCR;
    SPSR = _SPSR;
    #endif
    SPIBusState = false;
}

// Checks if status register 1 can be accessed - used to check chip status, during powerdown and power up and for debugging
uint8_t SPIFlash::_readStat1(void)
{
    _beginSPI(READSTAT1);
    stat1 = _nextByte();
    CHIP_DESELECT
    return stat1;
}

// Checks if status register 2 can be accessed, if yes, reads and returns it
uint8_t SPIFlash::_readStat2(void)
{
    _beginSPI(READSTAT2);
    stat2 = _nextByte();
    // stat2 = _nextByte();
    CHIP_DESELECT
    return stat2;
}

// Checks if status register 3 can be accessed, if yes, reads and returns it
uint8_t SPIFlash::_readStat3(void)
{
    _beginSPI(READSTAT3);
    stat3 = _nextByte();
    // stat2 = _nextByte();
    CHIP_DESELECT
    return stat3;
}

// Checks to see if 4-byte addressing is already enabled and if not, enables it
bool SPIFlash::_enable4ByteAddressing(void)
{
    if (_readStat3() & ADS) {
        return true;
    }
    _beginSPI(ADDR4BYTE_EN);
    CHIP_DESELECT
    if (_readStat3() & ADS) {
        address4ByteEnabled = true;
        return true;
    } else {
        _troubleshoot(UNABLETO4BYTE);
        return false;
    }
}

// Checks to see if 4-byte addressing is already disabled and if not, disables it
bool SPIFlash::_disable4ByteAddressing(void)
{
    if (!(_readStat3() & ADS)) { // If 4 byte addressing is disabled (default state)
        return true;
    }
    _beginSPI(ADDR4BYTE_DIS);
    CHIP_DESELECT
    if (_readStat3() & ADS) {
        _troubleshoot(UNABLETO3BYTE);
        return false;
    }
    address4ByteEnabled = false;
    return true;
}

// Checks the erase/program suspend flag before enabling/disabling a program/erase suspend operation
bool SPIFlash::_noSuspend(void)
{
    switch (_chip.manufacturerID) {
        case WINBOND_MANID:
            if (_readStat2() & SUS) {
                _troubleshoot(SYSSUSPEND);
                return false;
            }
            return true;

            break;

        case MICROCHIP_MANID:
            _readStat1();
            if (stat1 & WSE || stat1 & WSP) {
                _troubleshoot(SYSSUSPEND);
                return false;
            }
    }
    return true;
}

// Checks to see if chip is powered down. If it is, retrns true. If not, returns false.
bool SPIFlash::_isChipPoweredDown(void)
{
    if (chipPoweredDown) {
        _troubleshoot(CHIPISPOWEREDDOWN);
        return true;
    } else {
        return false;
    }
}

// Polls the status register 1 until busy flag is cleared or timeout
bool SPIFlash::_notBusy(uint32_t timeout)
{
    _delay_us(WINBOND_WRITE_DELAY);
    uint32_t _time = micros();

    do {
        _readStat1();
        if (!(stat1 & BUSY)) {
            return true;
        }
    } while ((micros() - _time) < timeout);
    if (timeout <= (micros() - _time)) {
        _troubleshoot(CHIPBUSY);
        return false;
    }
    return true;
}

// Enables writing to chip by setting the WRITEENABLE bit
bool SPIFlash::_writeEnable(bool _troubleshootEnable)
{
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

// Disables writing to chip by setting the Write Enable Latch (WEL) bit in the Status Register to 0
// _writeDisable() is not required under the following conditions because the Write Enable Latch (WEL) flag is cleared to 0
// i.e. to write disable state:
// Power-up, Write Disable, Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register,
// Erase Security Register and Program Security register
bool SPIFlash::_writeDisable(void)
{
    _beginSPI(WRITEDISABLE);
    CHIP_DESELECT
    return true;
}

bool SPIFlash::_disableGlobalBlockProtect(void)
{
    if (_chip.memoryTypeID == SST25_MEMID) {
        _readStat1();
        uint8_t _tempStat1 = stat1 & 0xC3;
        _beginSPI(WRITESTATEN);
        CHIP_DESELECT
          _beginSPI(WRITESTAT1);
        _nextByte(_tempStat1);
        CHIP_DESELECT
    } else if (_chip.memoryTypeID == SST26_MEMID) {
        if (!_notBusy()) {
            return false;
        }
        _writeEnable();
        _delay_us(10);
        _beginSPI(ULBPR);
        CHIP_DESELECT
          _delay_us(50);
        _writeDisable();
    }
    return true;
}

// Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of this file.
void SPIFlash::_troubleshoot(uint8_t _code, bool printoverride)
{
    diagnostics.troubleshoot(_code, printoverride);
}
