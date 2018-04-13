/* Arduino SPIFlash Library v.3.1.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 24/02/2018
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

// Functions that retrieve date from the SFDP tables - if the flash chip supports SFDP

// This function returns the SFDP table requested as an array of four 32 bit integers
bool SPIFlash::_getSFDPTable(uint32_t _address, uint8_t *data_buffer, uint8_t numberOfDWords) {
  if(!_notBusy()) {
  	return false;
  }

  _currentAddress = _address;
  _beginSPI(READSFDP);
  _transferAddress();
  _nextByte(WRITE, DUMMYBYTE);
  _nextBuf(READDATA, &(*data_buffer), numberOfDWords*4); //*4 to convert from dWords to bytes
  CHIP_DESELECT
  return true;
}

//dWordNumber can be between 1 to 256
uint32_t SPIFlash::_getSFDPdword(uint32_t _address, uint8_t dWordNumber) {
  if(!_notBusy()) {
  	return false;
  }
  union {
    uint32_t dWord;
    uint8_t byteArray[4];
  } SFDPdata;
  _currentAddress = ADDRESSOFSFDPDWORD(_address, dWordNumber);
  _beginSPI(READSFDP);
  _transferAddress();
  _nextByte(WRITE, DUMMYBYTE);
  _nextBuf(READDATA, &(*SFDPdata.byteArray), 0x04); //*4 bytes in a dWord
  return SFDPdata.dWord;
}

//byteNumber can be between 1 to 256
uint16_t SPIFlash::_getSFDPword(uint32_t _address, uint8_t dWordNumber, uint8_t startByte) {
  if(!_notBusy()) {
  	return false;
  }
  union {
    uint16_t word;
    uint8_t byteArray[2];
  } SFDPdata;
  _currentAddress = ADDRESSOFSFDPDWORD(_address, dWordNumber);
  _beginSPI(READSFDP);
  _transferAddress();
  _nextByte(WRITE, DUMMYBYTE);
  _nextBuf(READDATA, &(*SFDPdata.byteArray), 0x02); //*2 bytes in a word
  return SFDPdata.word;
}

//byteNumber can be between 1 to 256
uint8_t SPIFlash::_getSFDPbyte(uint32_t _address, uint8_t dWordNumber, uint8_t byteNumber) {
  if(!_notBusy()) {
  	return false;
  }
  uint8_t SFDPdata;
  _currentAddress = ADDRESSOFSFDPBYTE(_address, dWordNumber, byteNumber);
  _beginSPI(READSFDP);
  _transferAddress();
  _nextByte(WRITE, DUMMYBYTE);
  SFDPdata = _nextByte(READ);
  return SFDPdata;
}

//bitNumber can be between 0 to 31
bool SPIFlash::_getSFDPbit(uint32_t _address, uint8_t dWordNumber, uint8_t bitNumber) {
  return(_getSFDPdword(_address, dWordNumber) & (0x01 << bitNumber));
}

uint32_t SPIFlash::_getSFDPTableAddr(uint32_t paramHeaderNum) {
  uint32_t _tableAddr = _getSFDPdword(paramHeaderNum * 8, 0x02); // Each parameter header table is 8 bytes long
  //Serial.print("0x");
  //Serial.println(_tableAddr, HEX);
  Highest(_tableAddr) = 0x00; // Top byte in the dWord containing the table address is always 0xFF.
  return _tableAddr;
}

bool SPIFlash::_checkForSFDP(void) {
  if (_getSFDPdword(SFDP_HEADER_ADDR, SFDP_SIGNATURE_DWORD) == SFDPSIGNATURE) {
    sdfpPresent = true;
    #ifdef RUNDIAGNOSTIC
    Serial.println("SFDP available");
    #endif
  }
  else {
    sdfpPresent = false;
  }
  return sdfpPresent;
}

bool SPIFlash::_readSFDPParam(void) {
  _noOfParamHeaders = _getSFDPbyte(SFDP_HEADER_ADDR, SFDP_NPH_DWORD, SFDP_NPH_BYTE) + 1; // Number of parameter headers is 0 based - i.e. 0x00 means there is 1 header.
  //Serial.print("NPH: ");
  //Serial.println(_noOfParamHeaders);
  if (_noOfParamHeaders > 1) {
    _SectorMapParamTableAddr = _getSFDPTableAddr(SFDP_SECTOR_MAP_PARAM_TABLE_NO);
    #ifdef RUNDIAGNOSTIC
    //Serial.print("Sector Map Parameter Address: 0x");
    //Serial.println(_SectorMapParamTableAddr, HEX);
    #endif
  }
  _BasicParamTableAddr = _getSFDPTableAddr(SFDP_BASIC_PARAM_TABLE_NO);

// Calculate chip capacity
  _chip.capacity = _getSFDPdword(_BasicParamTableAddr, SFDP_MEMORY_DENSITY_DWORD);
  uint8_t _multiplier = Highest(_chip.capacity);
  Highest(_chip.capacity) = 0x00;
  if (_multiplier <= 0x0F) {
    _chip.capacity = (_chip.capacity + 1) * (_multiplier + 1);
  }
  else {
    _chip.capacity = ((_chip.capacity + 1) * 2);
  }
  if (!_chip.capacity) {
    _troubleshoot(UNKNOWNCAP);
    return false;
  }
  else {
    _chip.supported = true;
  }
  return true;
// Identify manufacturer using JEDEC ID only if required
}

// Reads and stores any required values from the Basic Flash Parameter table
bool SPIFlash::_getSFDPFlashParam(void) {
  uint32_t _dWord;
  uint8_t _byte;
  _byte = _getSFDPbyte(_BasicParamTableAddr, 0x01, 0x01);
  if (bit_is_set(_byte,0) && bit_is_clear(_byte,1)) { //If 4KB erase is supported
    Serial.print("4KB Erase command: 0x");
    Serial.println(_getSFDPbyte(_BasicParamTableAddr, 0x01, SFDP_4KB_ERASE_BYTE), HEX);

    Serial.print("dWord 01: 0b");
    Serial.println(_getSFDPdword(_BasicParamTableAddr, 0x01), HEX);
  }
  //(_getSFDPbit(_BasicParamTableAddr, 0x01, int bitNumber));
}

// Reads and stores any required values from the Basic Flash Parameter table
bool SPIFlash::_getSFDPFlashInstructions(void) {

}
