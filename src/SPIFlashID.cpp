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

/************************************************
* Private functions used to ID SPI Flash chips *
************************************************/

/**
 * @brief   Requests JEDEC ID from chip
 * Checks for the presence of chip by requesting  the JEDEC ID
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _chipID
 * @see   begin
 */
bool SPIFlash::_getJedecId(void)
{
    if (!_notBusy()) {
        return false;
    }
    _beginSPI(JEDECID);
    _chip.manufacturerID = _nextByte(); // manufacturer id
    _chip.memoryTypeID   = _nextByte(); // memory type
    _chip.capacityID     = _nextByte(); // capacity
    CHIP_DESELECT
    if (!_chip.manufacturerID) {
        _troubleshoot(NORESPONSE);
        return false;
    } else {
        return true;
    }
}

/**
 * @brief   Gets device Manufacturer ID
 * Checks the device manufacturer ID to establish storage parameters
 *
 * @param  b1 Pointer to an unsigned byte to return the memoryTypeID to
 * @param  b2 Pointer to an unsigned byte to return the capacityID to
 * @returns  TRUE if successful, FALSE if unsuccessful
 */
bool SPIFlash::_getManId(uint8_t * b1, uint8_t * b2)
{
    if (!_notBusy()) {
        return false;
    }
    _beginSPI(MANID);
    _nextByte();
    _nextByte();
    _nextByte();
    *b1 = _nextByte();
    *b2 = _nextByte();
    CHIP_DESELECT
    return true;
}

/**
 * @brief   Identifies Adesto manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idAdesto(void)
{
    switch (_chip.capacityID) {
        case AT25SF041:
            _chip.capacity = KB(512);
            return true;

            break;

        default:
            break;
    }
    return false;
}

/**
 * @brief   Identifies AMIC manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idAmic(void)
{
    switch (_chip.capacityID) {
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
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
}

/**
 * @brief   Identifies Cypress manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idCypress(void)
{
    switch (_chip.capacityID) {
        case S25FL116K: // or S25FL032P - same capacityID, different capacities
            if (_chip.memoryTypeID == S25FL032P_MEMID) {
                _chip.capacity = MB(4);
            } else {
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
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
} // SPIFlash::_idCypress

/**
 * @brief   Identifies Giga Devices manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idGiga(void)
{
    switch (_chip.capacityID) {
        case GD25Q16C:
            _chip.capacity = KB(512);
            return true;

            break;

        default:
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
}

/**
 * @brief   Identifies Macronix manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idMacronix(void)
{
    switch (_chip.capacityID) {
        case MX25L4005:
            _chip.capacity = KB(512);
            return true;

            break;

        case MX25L8005:
            _chip.capacity = MB(1);
            return true;

            break;

        default:
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
}

/**
 * @brief   Identifies Microchip manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idMicrochip(void)
{
    switch (_chip.memoryTypeID) {
        case SST25_MEMID:
            if (_chip.capacityID == SST25VF064C) {
                _chip.capacity = MB(8);
                return true;
            } else {
                return false;
            }
            break;

        case SST26_MEMID:
            switch (_chip.capacityID) {
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

                default:
                    _chip.capacity = MB(1 << (_chip.capacityID));
                    return true;

                    break;
            }

        default:
            return false;

            break;
    }
    return false;
} // SPIFlash::_idMicrochip

/**
 * @brief   Identifies Micron manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idMicron(void)
{
    switch (_chip.capacityID) {
        case M25P40:
            _chip.capacity = KB(512);
            return true;

            break;

        case M25P80:
            _chip.capacity = MB(1);
            return true;

            break;

        default:
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
}

/**
 * @brief   Identifies ON Semiconductor manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idOn(void)
{
    switch (_chip.capacityID) {
        case LE25U40CMC:
            _chip.capacity = KB(512);
            return true;

            break;

        default:
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
}

/**
 * @brief   Identifies Winbond manufactured flash memories' storage capacity
 *
 * @returns TRUE if successful, FALSE if unsuccessful
 * @see   _getJedecId
 * @see   _chipID
 */
bool SPIFlash::_idWinbond(void)
{
    switch (_chip.capacityID) {
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
            _chip.capacity = 1 << (_chip.capacityID);
            return true;

            break;
    }
    return false;
} // SPIFlash::_idWinbond

/**
 * @brief   Identifies the flash memory chip
 * Identifies the flash memory chip using one of three methods:
 *      1.  Uses user-declared custom chipsize to set up library.
 *          If `flashChipSize` is declared, it overrides automatic identification.
 *      2.  Uses sfdp parameters to set up library.
 *          If `#define USES_SFDP` is declared, it overrides the library support check.
 *      3.    Gets manufacturer ID from the `_getJedecId()` function.
 *          Uses it to check if the chip is supported by the library.
 *          Attempts to identify unsupported chips if they are from supported manufacturers.
 *
 * @param   flashChipSize Optional uint32_t passed on from `begin()`. Allows end-user to declare a custom chip-size
 * @returns TRUE if chip is successfully identified. FALSE if not.
 * @throws  `UNKNOWNCHIP` if chip is from a supported manufacturer but capacity cannot be identified
 * @throws  `UNKNOWNCAP` if chip's capacity cannot be identified.
 * @exceptsafe  **basic** If the function throws an exception, the program will be in a valid state, but not necessarily a predictable one. No memory, file descriptors, locks, or other resources will be leaked.
 * @see   _getJedecId
 * @see   _getManId
 * @see   _idAdesto
 * @see   _idAmic
 * @see   _idCypress
 * @see   _idGiga
 * @see   _idMacronix
 * @see   _idMicrochip
 * @see   _idMicron
 * @see   _idOn
 * @see   _idWinbond
 */
bool SPIFlash::_chipID(uint32_t flashChipSize)
{
    if (_getJedecId()) {
        chipPoweredDown = false; // If comms can be established, chip is awake
    } else {
        return false; // SPI comms failure
    }

    /*********************************
    * Begin custom chipsize section *
    *********************************/
    if (flashChipSize) {
        #ifdef RUNDIAGNOSTIC
        Serial.println(F("Custom Chipsize defined"));
        #endif
        _chip.capacity  = flashChipSize;
        _chip.supported = false; // Chip might not be officially supported by the library
        _setDefaultParams();     // Sets chip parameters to defaults
        return true;
    } else {
        #ifdef RUNDIAGNOSTIC
        Serial.println(F("No Chip size defined by user. Automatic ID initiated. "));
        #endif
    }

    /*******************************
    * End custom chipsize section *
    *******************************/


    /*************************
    * Begin SFDP ID section *
    *************************/
    #ifdef USES_SFDP

    # ifdef RUNDIAGNOSTIC
    Serial.println(F("Checking SFDP ID."));
    # endif
    if (_checkForSFDP()) {
        _getSFDPFlashParam();
        # ifdef RUNDIAGNOSTIC
        Serial.println(F("SFDP ID finished."));
        # endif
        return true;
    } else {
        _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
        return false;
    }
    #else // ifdef USES_SFDP

    # ifdef RUNDIAGNOSTIC
    Serial.println(F("SFDP ID not requested"));
    # endif

    #endif // ifdef USES_SFDP

    /***********************
    * End SFDP ID section *
    ***********************/


    /***************************************
    * Begin Library support check section *
    ***************************************/
    #ifdef RUNDIAGNOSTIC
    Serial.println(F("Checking library support."));
    #endif
    _setDefaultParams(); // Sets chip parameters to defaults
    switch (_chip.manufacturerID) {
        case ADESTO_MANID:
            _chip.supportedMan = true;
            if (!_idAdesto()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case AMIC_MANID:
            _chip.supportedMan = true;
            chipErase.opcode   = ALT_CHIPERASE; // AMIC chips use 0xC7 instead of 0x60
            if (!_idAmic()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case CYPRESS_MANID:
            _chip.supportedMan = true;
            if (!_idCypress()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case GIGA_MANID:
            _chip.supportedMan = true;
            if (!_idGiga()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case MACRONIX_MANID:
            _chip.supportedMan = true;
            if (!_idMacronix()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case MICROCHIP_MANID:
            _chip.supportedMan = true;
            if (_chip.memoryTypeID == SST26_MEMID) {
                chipErase.opcode = ALT_CHIPERASE; // SST26 chips use 0xC7 instead of 0x60
            }
            if (!_idMicrochip()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case MICRON_MANID:
            _chip.supportedMan = true;
            chipErase.opcode   = ALT_CHIPERASE; // Micron chips use 0xC7 instead of 0x60
            if (!_idMicron()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case ON_MANID:
            _chip.supportedMan = true;
            if (!_idMacronix()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        case WINBOND_MANID:
            _chip.supportedMan = true;
            if (!_idWinbond()) {
                _troubleshoot(UNKNOWNCHIP); // Error code for unidentified capacity
            } else {
                _chip.supported = true;
            }
            break;

        default:
            _chip.supportedMan = false;
            _chip.supported    = false;
            break;
    }

    /***************************************
    * Begin Library support check section *
    ***************************************/

    if (_chip.supported) {
        #ifdef RUNDIAGNOSTIC
        Serial.println(F("Chip identified. This chip is fully supported by the library."));
        #endif
        return true;
    } else {
        _troubleshoot(UNKNOWNCHIP);
        return false;
    }

    if (!_chip.capacity) {
        _troubleshoot(UNKNOWNCAP);
        return false;
    }

    return true;
} // SPIFlash::_chipID
