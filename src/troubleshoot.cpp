/* Arduino SPIFlash Library v.2.7.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 14/11/2016
 * Modified by @boseji <salearj@hotmail.com> - 02/03/2017
 * Modified by Prajwal Bhattaram - 02/08/2017
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

 #if !defined (__AVR_ATtiny85__)
 //Subfunctions for troubleshooting function
 void SPIFlash::_printErrorCode(void) {
   Serial.print("Error code: 0x");
   if (errorcode < 0x10) {
     Serial.print("0");
   }
   Serial.println(errorcode, HEX);
 }

 void SPIFlash::_printSupportLink(void) {
   Serial.print("If this does not help resolve/clarify this issue, ");
   Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 }
 //Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of this file.
 void SPIFlash::_troubleshoot(void) {

 	switch (errorcode) {
    case SUCCESS:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Action completed successfully");
  #endif
    break;

    case NORESPONSE:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Check your wiring. Flash chip is non-responsive.");
    _printSupportLink();
  #endif
    break;

    case CALLBEGIN:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("*constructor_of_choice*.begin() was not called in void setup()");
    _printSupportLink();
  #endif
    break;

    case UNKNOWNCHIP:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    Serial.print("Error code: 0x0");
    Serial.println(UNKNOWNCHIP, HEX);
  #else
    Serial.println("Unable to identify chip. Are you sure this chip is supported?");
    _printSupportLink();
  #endif
  Serial.println("Chip details:");
    Serial.print("manufacturer ID: 0x"); Serial.println(_chip.manufacturerID, HEX);
    Serial.print("capacity ID: 0x");Serial.println(_chip.memoryTypeID, HEX);
    Serial.print("device ID: 0x");Serial.println(_chip.capacityID, HEX);
    break;

    case UNKNOWNCAP:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Unable to identify capacity. Is this chip officially supported? If not, please define a `CAPACITY` constant and include it in flash.begin(CAPACITY).");
    _printSupportLink();
  #endif
    break;

    case CHIPBUSY:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Chip is busy.");
    Serial.println("Make sure all pins have been connected properly");
    _printSupportLink();
  #endif
    break;

    case OUTOFBOUNDS:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Page overflow has been disabled and the address called exceeds the memory");
    _printSupportLink();
  #endif
    break;

    case CANTENWRITE:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Unable to Enable Writing to chip.");
    Serial.println("Please make sure the HOLD & WRITEPROTECT pins are pulled up to VCC");
    _printSupportLink();
  #endif
    break;

    case PREVWRITTEN:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("This sector already contains data.");
    Serial.println("Please make sure the sectors being written to are erased.");
    _printSupportLink();
  #endif
    break;

    case LOWRAM:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("You are running low on SRAM. Please optimise your program for better RAM usage");
    /*#if defined (ARDUINO_ARCH_SAM)
      Serial.print("Current Free SRAM: ");
      Serial.println(freeRAM());
    #endif*/
    _printSupportLink();
  #endif
    break;

    case SYSSUSPEND:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Unable to suspend/resume operation.");
    _printSupportLink();
  #endif
    break;

    case UNSUPPORTED:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("This function is not supported by the current flash IC.");
    _printSupportLink();
  #endif
    break;

  case ERRORCHKFAIL:
#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  _printErrorCode();
#else
  Serial.println("Write Function has failed errorcheck.");
  _printSupportLink();
#endif
  break;

    default:
  #if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
    _printErrorCode();
  #else
    Serial.println("Unknown error");
    _printSupportLink();
  #endif
  break;
 	}
 }
 #endif
