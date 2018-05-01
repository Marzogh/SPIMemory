/* Arduino SPIMemory Library v.3.1.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 24/02/2018
 *
 * This file is part of the Arduino SPIMemory Library. This library is for
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
 * along with the Arduino SPIMemory Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

 #include "SPIFlash.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     Private Functions that retrieve date from the SFDP tables      //
//              - if the flash chip supports SFDP                     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

// This function returns the SFDP table requested as an array of 32 bit integers
bool SPIFlash::_getSFDPTable(uint32_t _address, uint8_t *data_buffer, uint8_t numberOfDWords) {
  if(!_notBusy()) {
   return false;
  }
  _beginSPI(READSFDP);
  _nextByte(WRITE, Higher(_address));
  _nextByte(WRITE, Hi(_address));
  _nextByte(WRITE, Lo(_address));
  _nextByte(WRITE, DUMMYBYTE);
  _nextBuf(READDATA, &(*data_buffer), numberOfDWords*4); //*4 to convert from dWords to bytes
  CHIP_DESELECT
  return true;
}

// This function returns a custom length of data from the SFDP table requested as an array of 8 bit integers (bytes)
bool SPIFlash::_getSFDPData(uint32_t _address, uint8_t *data_buffer, uint8_t numberOfBytes) {
  if(!_notBusy()) {
   return false;
  }
  _beginSPI(READSFDP);
  _nextByte(WRITE, Higher(_address));
  _nextByte(WRITE, Hi(_address));
  _nextByte(WRITE, Lo(_address));
  _nextByte(WRITE, DUMMYBYTE);
  _nextBuf(READDATA, &(*data_buffer), numberOfBytes); //*4 to convert from dWords to bytes
  CHIP_DESELECT
  return true;
}

//dWordNumber can be between 1 to 256
uint32_t SPIFlash::_getSFDPdword(uint32_t _tableAddress, uint8_t dWordNumber) {
  if(!_notBusy()) {
   return false;
  }
  union {
    uint32_t dWord;
    uint8_t byteArray[4];
  } SFDPdata;
  uint32_t _address = ADDRESSOFSFDPDWORD(_tableAddress, dWordNumber);
  _getSFDPData(_address, &(*SFDPdata.byteArray), sizeof(uint32_t));
  return SFDPdata.dWord;
}

//startByte is the byte from which the 16-bit integer starts and can be between 1 to 256
uint16_t SPIFlash::_getSFDPint(uint32_t _tableAddress, uint8_t dWordNumber, uint8_t startByte) {
  if(!_notBusy()) {
   return false;
  }
  union {
    uint16_t word;
    uint8_t byteArray[2];
  } SFDPdata;
  uint32_t _address = ADDRESSOFSFDPBYTE(_tableAddress, dWordNumber, startByte);
  _getSFDPData(_address, &(*SFDPdata.byteArray), sizeof(uint16_t));
  return SFDPdata.word;
}

//byteNumber can be between 1 to 256
uint8_t SPIFlash::_getSFDPbyte(uint32_t _tableAddress, uint8_t dWordNumber, uint8_t byteNumber) {
  if(!_notBusy()) {
   return false;
  }
  uint8_t SFDPdataByte;
  uint32_t _address = ADDRESSOFSFDPBYTE(_tableAddress, dWordNumber, byteNumber);
  _getSFDPData(_address, &SFDPdataByte, sizeof(uint8_t));
  return SFDPdataByte;
}

//bitNumber can be between 0 to 31
bool SPIFlash::_getSFDPbit(uint32_t _tableAddress, uint8_t dWordNumber, uint8_t bitNumber) {
  return(_getSFDPdword(_tableAddress, dWordNumber) & (0x01 << bitNumber));
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
    _chip.sfdpAvailable = true;
    #ifdef RUNDIAGNOSTIC
    Serial.println("SFDP available");
    #endif
  }
  else {
    _troubleshoot(NOSFDP);
    _chip.sfdpAvailable = false;
  }
  return _chip.sfdpAvailable;
}
uint32_t SPIFlash::_calcSFDPEraseTimeUnits(uint8_t _unitBits) {
  switch (_unitBits) {
    case MS1:
    return 1000L;
    break;

    case MS16:
    return (16L*1000L);
    break;

    case MS128:
    return (128L*1000L);
    break;

    case S1:
    return (1000L*1000L);
  }
  return false;
}

bool SPIFlash::_getSFDPEraseParam(void) {
  uint32_t _eraseInfoAddress;
  uint8_t _eraseInfo[8];
  uint8_t _eraseExists = 0;
  uint8_t _count;
  uint32_t _units;
  union {
    uint32_t dword;
    uint8_t byte[4];
  } _eraseTime;

  // Get sector erase details
  _eraseInfoAddress = ADDRESSOFSFDPDWORD(_BasicParamTableAddr, DWORD(8));
  _getSFDPData(_eraseInfoAddress, &(*_eraseInfo), 8);

  for (uint8_t i = 0; i < 8; i++) {
    _eraseExists += _eraseInfo[i];
  }

  if (!_eraseExists) {
    _troubleshoot(NOSFDPERASEPARAM);
    return false;
  }
  else {
    if (_noOfBasicParamDwords > 9) {                   // Some flash memory chips have information abut erase times in DWORD 10 & 11 of SFDP Basic param table.
      _eraseTime.dword = _getSFDPdword(_BasicParamTableAddr, DWORD(10));
      _eraseTimeMultiplier = _eraseTime.byte[0];
      setUpperNibble(_eraseTimeMultiplier, 0b0000);
      _eraseTimeMultiplier = 2 * (_eraseTimeMultiplier + 1);  // Refer JESD216B Page 21
      //Serial.print("Erase time Multiplier: ");
      //Serial.println(_eraseTimeMultiplier);

      for (uint8_t i = 0; i < 8; i++) {
        if ((i % 2) == 0) {
          switch ((_eraseInfo[i])) {
            case KB4ERASE_TYPE:
            kb4Erase.supported = true;
            kb4Erase.opcode = _eraseInfo[i+1];
            _count = (((_eraseTime.byte[1] & _createMask(0, 0)) << 5) | ((_eraseTime.byte[0] & _createMask(4, 7)) >> 4) + 1);
            _units = _calcSFDPEraseTimeUnits((_eraseTime.byte[1] & _createMask(1, 2)) >> 1);
            kb4Erase.time = (_count * _units * _eraseTimeMultiplier);
            break;

            case KB32ERASE_TYPE:
            kb32Erase.supported = true;
            kb32Erase.opcode = _eraseInfo[i+1];
            _count = (((_eraseTime.byte[1] & _createMask(3, 7)) >> 3) + 1);
            _units = _calcSFDPEraseTimeUnits(_eraseTime.byte[2] & _createMask(0, 1));
            kb32Erase.time = (_count * _units * _eraseTimeMultiplier);
            break;

            case KB64ERASE_TYPE:
            kb64Erase.supported = true;
            kb64Erase.opcode = _eraseInfo[i+1];
            _count = (((_eraseTime.byte[2] & _createMask(2, 6)) >> 2) + 1);
            _units = _calcSFDPEraseTimeUnits(((_eraseTime.byte[2] & _createMask(7, 7)) >> 7) | (_eraseTime.byte[3] & _createMask(0,0)) << 1);
            kb64Erase.time = (_count * _units * _eraseTimeMultiplier);
            break;

            case KB256ERASE_TYPE:
            kb256Erase.supported = true;
            kb256Erase.opcode = _eraseInfo[i+1];
            _count = (((_eraseTime.byte[3] & _createMask(1, 5)) >> 1) + 1);
            _units = _calcSFDPEraseTimeUnits((_eraseTime.byte[3] & _createMask(6, 7)) >> 6);
            kb64Erase.time = (_count * _units) * _eraseTimeMultiplier;
            break;
          }
        }
      }
      if (_noOfBasicParamDwords > 10) {
        // Get chip erase details
        _eraseInfoAddress = 0; // Reset variables
        _eraseInfoAddress = ADDRESSOFSFDPDWORD(_BasicParamTableAddr, DWORD(11));
        Serial.print("_eraseInfoAddress: 0x");
        Serial.println(_eraseInfoAddress, HEX);
        _getSFDPData(_eraseInfoAddress, &(*_eraseInfo), 8);
        chipErase.supported = true; // chipErase.opcode is set in _chipID().
        _count = (((_eraseTime.byte[3] & _createMask(0, 4))) + 1);
        _units = _calcSFDPEraseTimeUnits((_eraseTime.byte[3] & _createMask(5, 6)) >> 5);
        chipErase.time = (_count * _units) * _eraseTimeMultiplier;
        Serial.print("chipErase.time");
        Serial.println(chipErase.time);
      }
    }
    else {                                //If flash memory does not have any sfdp information about erase times
      if (_noOfBasicParamDwords < 11) {
        _troubleshoot(NOSFDPCHIPERASETIME);
        chipErase.opcode = CHIPERASE;
        chipErase.time = kb64Erase.time * 100L;
      }
      if (_noOfBasicParamDwords < 10) {
        _troubleshoot(NOSFDPSECTORERASETIME);
        kb4Erase.time = BUSY_TIMEOUT;
        kb32Erase.time = kb4Erase.time * 8;
        kb64Erase.time = kb32Erase.time * 4;
        kb256Erase.supported = false;
      }
    }
  }
  return true;
}

// Reads and stores any required values from the Basic Flash Parameter table
bool SPIFlash::_getSFDPFlashParam(void) {
    _noOfParamHeaders = _getSFDPbyte(SFDP_HEADER_ADDR, SFDP_NPH_DWORD, SFDP_NPH_BYTE) + 1; // Number of parameter headers is 0 based - i.e. 0x00 means there is 1 header.
    Serial.print("No. of Parameter headers: ");
    Serial.println(_noOfParamHeaders);
    if (_noOfParamHeaders > 1) {
      _SectorMapParamTableAddr = _getSFDPTableAddr(SFDP_SECTOR_MAP_PARAM_TABLE_NO);
      #ifdef RUNDIAGNOSTIC
      Serial.print("Sector Map Parameter Address: 0x");
      Serial.println(_SectorMapParamTableAddr, HEX);
      #endif
    }
    _noOfBasicParamDwords = _getSFDPbyte(SFDP_BASIC_PARAM_TABLE_HDR_ADDR, PARAM_TABLE_LENGTH_DWORD, PARAM_TABLE_LENGTH_BYTE);
    _BasicParamTableAddr = _getSFDPTableAddr(SFDP_BASIC_PARAM_TABLE_NO);
    #ifdef RUNDIAGNOSTIC
    Serial.print("SFDP Basic Parameter Address: 0x");
    Serial.println(_BasicParamTableAddr, HEX);
    #endif
    // Calculate chip capacity
    _chip.capacity = _getSFDPdword(_BasicParamTableAddr, SFDP_MEMORY_DENSITY_DWORD);
    uint8_t _multiplier = Highest(_chip.capacity);                  //----
    Highest(_chip.capacity) = 0x00;                  //                   |
    if (_multiplier <= 0x0F) {                       //                   |
      _chip.capacity = (_chip.capacity + 1) * (_multiplier + 1); //       |---> This section calculates the chip capacity as
    }                                                //                   |---> detailed in JESD216B
    else {                                           //                   |
      _chip.capacity = ((_chip.capacity + 1) * 2);   //                   |
    }                                                              //----
    #ifdef RUNDIAGNOSTIC
      Serial.println("Chip identified using sfdp. Most of this chip's functions are supported by the library.");
    #endif

  // Get Erase Parameters if available
  _getSFDPEraseParam();

  Serial.print("Parameter Header 01: 0x");
  Serial.println(_getSFDPdword(0x00, DWORD(4)), HEX);
  Serial.print("dWord 10: 0x");
  Serial.println(_getSFDPdword(_BasicParamTableAddr, DWORD(10)), HEX);
  Serial.print("dWord 9: 0x");
  Serial.println(_getSFDPdword(_BasicParamTableAddr, DWORD(9)), HEX);
  //(_getSFDPbit(_BasicParamTableAddr, 0x01, int bitNumber));
  return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End SFDP ID section ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
