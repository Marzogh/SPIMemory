/* Arduino SPIMemory Library v.3.4.0
 * Copyright (C) 2019 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 14/11/2016
 * Modified by Prajwal Bhattaram - 03/06/2018
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

#include "diagnostics.h"

//Subfunctions for troubleshooting function
void Diagnostics::_printErrorCode(void) {
  Serial.print(F("Error code: 0x"));
  if (errorcode < 0x10) {
    Serial.print(F("0"));
  }
  Serial.println(errorcode, HEX);
}

void Diagnostics::_printSupportLink(void) {
  Serial.print(F("If this does not help resolve/clarify this issue, "));
  Serial.println(F("please raise an issue at http://www.github.com/Marzogh/SPIMemory/issues with the details of what your were doing when this error occurred"));
}
//Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of SPIMemory.h.
void Diagnostics::troubleshoot(uint8_t _code, bool printoverride) {
  bool _printoverride;
  errorcode = _code;
#if defined (RUNDIAGNOSTIC) && !defined (ARDUINO_ARCH_AVR)
  _printoverride = true;
#elif defined (RUNDIAGNOSTIC) && defined (ARDUINO_ARCH_AVR)
  _printErrorCode();
#endif
#if !defined (RUNDIAGNOSTIC)
  _printoverride = printoverride;
#endif
  if (_printoverride) {
  #if defined (ARDUINO_ARCH_AVR)
    _printErrorCode();
  #else
    switch (_code) {
      case SUCCESS:
      Serial.println(F("Function executed successfully"));
      break;

      case NORESPONSE:
      Serial.println(F("Check your wiring. Flash chip is non-responsive."));
      break;

      case CALLBEGIN:
      Serial.println(F("*constructor_of_choice*.begin() was not called in void setup()"));
      break;

      case UNKNOWNCHIP:
      Serial.println(F("Unable to identify chip. Are you sure this chip is supported?"));
      //Serial.println(F("Chip details:"));
      //TODO: Insert a diagnostics subroutine here.
      break;

      case UNKNOWNCAP:
      Serial.println(F("Unable to identify capacity. Is this chip officially supported? If not, please define a `CAPACITY` constant and include it in flash.begin(CAPACITY)."));
      break;

      case CHIPBUSY:
      Serial.println(F("Chip is busy."));
      Serial.println(F("Make sure all pins have been connected properly"));
      break;

      case OUTOFBOUNDS:
      Serial.println(F("Page overflow has been disabled and the address called exceeds the memory"));
      break;

      case CANTENWRITE:
      Serial.println(F("Unable to Enable Writing to chip."));
      Serial.println(F("Please make sure the HOLD & WRITEPROTECT pins are pulled up to VCC"));
      break;

      case PREVWRITTEN:
      Serial.println(F("This sector already contains data."));
      Serial.println(F("Please make sure the sectors being written to are erased."));
      break;

      case LOWRAM:
      Serial.println(F("You are running low on SRAM. Please optimise your program for better RAM usage"));
      /*#if defined (ARDUINO_ARCH_SAM)
        Serial.print(F("Current Free SRAM: "));
        Serial.println(freeRAM());
      #endif*/
      break;

      case UNSUPPORTEDFUNC:
      Serial.println(F("This function is not supported by the flash memory hardware."));
      break;

      case SYSSUSPEND:
      Serial.println(F("Unable to suspend/resume operation."));
      break;

      case ERRORCHKFAIL:
      Serial.println(F("Write Function has failed errorcheck."));
      break;

      case UNABLETO4BYTE:
      Serial.println(F("Unable to enable 4-byte addressing."));
      break;

      case UNABLETO3BYTE:
      Serial.println(F("Unable to disable 4-byte addressing."));
      break;

      case CHIPISPOWEREDDOWN:
      Serial.println(F("The chip is currently powered down."));
      break;

      case NOSFDP:
      Serial.println(F("The Flash chip does not support SFDP."));
      break;

      case NOSFDPERASEPARAM:
      Serial.println(F("Unable to read Erase Parameters from chip. Reverting to library defaults."));
      break;

      case NOSFDPERASETIME:
      Serial.println(F("Unable to read erase times from flash memory. Reverting to library defaults."));
      break;

      case NOSFDPPROGRAMTIMEPARAM:
      Serial.println(F("Unable to read program times from flash memory. Reverting to library defaults."));
      break;

      case NOCHIPSELECTDECLARED:
      Serial.println(F("No Chip Select pin defined in the custom SPI Array."));
      break;

      default:
      Serial.println(F("Unknown error"));
      break;
    }
    if (_code == ERRORCHKFAIL || _code == CANTENWRITE || _code == UNKNOWNCHIP || _code == NORESPONSE) {
      _printSupportLink();
    }
  #endif
  }
}

Diagnostics diagnostics; // default instantiation of Diagnostics object
