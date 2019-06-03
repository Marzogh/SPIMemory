/* Arduino SPIMemory Library v.3.5.0
 * Copyright (C) 2019 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 03/06/2019
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//           Private functions used to ID SPI Flash chips             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//Checks for presence of chip by requesting JEDEC ID
bool SPIFlash::_getJedecId(void) {
        if(!_notBusy()) {
                return false;
        }
        _beginSPI(JEDECID);
        _chip.manufacturerID = _nextByte(READ); // manufacturer id
        _chip.memoryTypeID = _nextByte(READ); // memory type
        _chip.capacityID = _nextByte(READ); // capacity
        CHIP_DESELECT
        if (!_chip.manufacturerID) {
                _troubleshoot(NORESPONSE);
                return false;
        }
        else {
                return true;
        }
}

//Checks the device ID to establish storage parameters
bool SPIFlash::_getManId(uint8_t *b1, uint8_t *b2) {
        if(!_notBusy()) {
                return false;
        }
        _beginSPI(MANID);
        _nextByte(READ);
        _nextByte(READ);
        _nextByte(READ);
        *b1 = _nextByte(READ);
        *b2 = _nextByte(READ);
        CHIP_DESELECT
        return true;
}

bool SPIFlash::_idAdesto(void) {
        switch(_chip.capacityID)
        {
        case AT25SF041:
                _chip.capacity = KB(512);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idAmic(void) {
        switch(_chip.capacityID)
        {
        case A25L512:
                _chip.capacity = KB(64);
                return true;
                break;

        case A25L010:
                _chip.capacity = KB(128);
                return true;
                break;

        case A25L020:
                _chip.capacity = KB(512);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idCypress(void) {
        switch(_chip.capacityID)
        {
        case S25FL116K:       // or S25FL032P - same capacityID, different capacities
                if (_chip.memoryTypeID == S25FL032P_MEMID) {
                        _chip.capacity = MB(4);
                }
                else {
                        _chip.capacity = MB(2);
                }
                return true;
                break;

        case S25FL132K:
                _chip.capacity = MB(4);
                return true;
                break;

        case S25FL164K:
                _chip.capacity = MB(8);
                return true;
                break;

        case S25FL127S:
                _chip.capacity = MB(16);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idGiga(void) {
        switch(_chip.capacityID)
        {
        case GD25Q16C:
                _chip.capacity = KB(512);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idMacronix(void) {
        switch(_chip.capacityID)
        {
        case MX25L4005:
                _chip.capacity = KB(512);
                return true;
                break;

        case MX25L8005:
                _chip.capacity = MB(1);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idMicrochip(void) {
        switch(_chip.capacityID)
        {
        case SST26VF016B:
                _chip.capacity = MB(2);
                return true;
                break;

        case SST26VF032B:
                _chip.capacity = MB(4);
                return true;
                break;

        case SST26VF064B:
                _chip.capacity = MB(8);
                return true;
                break;

        case SST25VF064C:
                _chip.capacity = MB(8);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idMicron(void) {
        switch(_chip.capacityID)
        {
        case M25P40:
                _chip.capacity = KB(512);
                return true;
                break;

        case M25P80:
                _chip.capacity = MB(1);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idOn(void) {
        switch(_chip.capacityID)
        {
        case LE25U40CMC:
                _chip.capacity = KB(512);
                return true;
                break;

        default:
                break;
        }
        return false;
}

bool SPIFlash::_idWinbond(void) {
        switch(_chip.capacityID)
        {
        case W25Q80BV:
                _chip.capacity = MB(1);
                return true;
                break;

        case W25Q16BV:
                _chip.capacity = MB(2);
                return true;
                break;

        case W25Q64BV:
                _chip.capacity = MB(8);
                return true;
                break;

        case W25Q256FV:
                _chip.capacity = MB(32);
                return true;
                break;

        default:
                break;
        }
        return false;
}

//Identifies the chip
bool SPIFlash::_chipID(uint32_t flashChipSize) {

// Checks to see if comms can be established and gets JEDEC ID
        if (_getJedecId()) {
                chipPoweredDown = false; // If comms can be established, chip is awake
        }
        else {
                return false;   // SPI comms failure
        }

//***************************************//
//~~~~ Begin custom chipsize section ~~~~//
//***************************************//
// Uses user-declared custom chipsize to set up library
// If custom chipsize is declared, it overrides automatic identification.

        if (flashChipSize) {
        #ifdef RUNDIAGNOSTIC
                Serial.println(F("Custom Chipsize defined"));
        #endif
                _chip.capacity = flashChipSize;
                _chip.supported = false; // Chip might not be officially supported by the library
                _setDefaultParams(); // Sets chip parameters to defaults
                return true;
        }
        else {
        #ifdef RUNDIAGNOSTIC
                Serial.println(F("No Chip size defined by user. Automatic ID initiated. "));
        #endif
        }

//*************************************//
//~~~~ End custom chipsize section ~~~~//
//*************************************//

//*******************************//
//~~~~ Begin SFDP ID section ~~~~//
//*******************************//
// Uses sfdp parameters to set up library
// If USES_SFDP is declared, it overrides the library support check.

 #ifdef USES_SFDP

   #ifdef RUNDIAGNOSTIC
        Serial.println(F("Checking SFDP ID."));
   #endif
        if (_checkForSFDP()) {
                _getSFDPFlashParam();
        #ifdef RUNDIAGNOSTIC
                Serial.println(F("SFDP ID finished."));
        #endif
                return true;
        }
        else {
                _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                return false;
        }
 #else

   #ifdef RUNDIAGNOSTIC
        Serial.println(F("SFDP ID not requested"));
   #endif

 #endif
//*****************************//
//~~~~ End SFDP ID section ~~~~//
//*****************************//

//*********************************************//
//~~~~ Begin Library support check section ~~~~//
//*********************************************//
// Uses the manufacturer ID obtained from the _getJedecId() function earlier
#ifdef RUNDIAGNOSTIC
        Serial.println(F("Checking library support."));
#endif
        _setDefaultParams(); // Sets chip parameters to defaults
        switch (_chip.manufacturerID) {

        case ADESTO_MANID:
                _chip.supportedMan = true;
                if (!_idAdesto()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case AMIC_MANID:
                _chip.supportedMan = true;
                chipErase.opcode = ALT_CHIPERASE; //AMIC chips use 0xC7 instead of 0x60
                if (!_idAmic()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case CYPRESS_MANID:
                _chip.supportedMan = true;
                if (!_idCypress()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case GIGA_MANID:
                _chip.supportedMan = true;
                if (!_idGiga()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case MACRONIX_MANID:
                _chip.supportedMan = true;
                if (!_idMacronix()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case MICROCHIP_MANID:
                _chip.supportedMan = true;
                if (_chip.memoryTypeID == SST26) {
                        chipErase.opcode = ALT_CHIPERASE; //SST26 chips use 0xC7 instead of 0x60
                }
                if (!_idMicrochip()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case MICRON_MANID:
                _chip.supportedMan = true;
                chipErase.opcode = ALT_CHIPERASE; //Micron chips use 0xC7 instead of 0x60
                if (!_idMicron()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case ON_MANID:
                _chip.supportedMan = true;
                if (!_idMacronix()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        case WINBOND_MANID:
                _chip.supportedMan = true;
                if (!_idWinbond()) {
                        _troubleshoot(UNKNOWNCHIP); //Error code for unidentified capacity
                }
                else {
                        _chip.supported = true;
                }
                break;

        default:
                _chip.supportedMan = false;
                _chip.supported = false;
                break;
        }


        if (_chip.supported) {
          #ifdef RUNDIAGNOSTIC
                Serial.println(F("Chip identified. This chip is fully supported by the library."));
          #endif
                return true;
        }
        else {
                _troubleshoot(UNKNOWNCHIP);
                return false;
        }

        if (!_chip.capacity) {
                _troubleshoot(UNKNOWNCAP);
                return false;
        }

        return true;
}
