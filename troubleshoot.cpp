/* Arduino SPIFlash Library v.2.5.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 14/11/2016
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

 #ifdef RUNDIAGNOSTIC
 //Troubleshooting function. Called when #ifdef RUNDIAGNOSTIC is uncommented at the top of this file.
 void SPIFlash::_troubleshoot(void) {

 	switch (errorcode) {
 		case SUCCESS:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(SUCCESS, HEX);
 		#else
 		Serial.println("Action completed successfully");
 		#endif
 		break;

  		case CALLBEGIN:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(CALLBEGIN, HEX);
 		#else
  		Serial.println("*constructor_of_choice*.begin() was not called in void setup()");
 		#endif
 		break;

 		case UNKNOWNCHIP:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
    Serial.print("manID: 0x"); Serial.println(manID, HEX);
    Serial.print("capID: 0x");Serial.println(capID, HEX);
    Serial.print("devID: 0x");Serial.println(devID, HEX);
 		Serial.println(UNKNOWNCHIP, HEX);
 		#else
 		Serial.println("Unable to identify chip. Are you sure this is a Winbond Flash chip");
    Serial.print("manID: 0x"); Serial.println(manID, HEX);
    Serial.print("capID: 0x");Serial.println(capID, HEX);
    Serial.print("devID: 0x");Serial.println(devID, HEX);
  		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with your chip type and I will try to add support to your chip");
 		#endif

 		break;

  		case UNKNOWNCAP:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(UNKNOWNCAP, HEX);
 		#else
  		Serial.println("Unable to identify capacity.");
  		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with your chip type and I will work on adding support to your chip");
 		#endif
 		break;

  		case CHIPBUSY:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(CHIPBUSY, HEX);
 		#else
  		Serial.println("Chip is busy.");
  		Serial.println("Make sure all pins have been connected properly");
  		Serial.print("If it still doesn't work, ");
  		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 		#endif
 		break;

  		case OUTOFBOUNDS:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(OUTOFBOUNDS, HEX);
 		#else
  		Serial.println("Page overflow has been disabled and the address called exceeds the memory");
 		#endif
 		break;

  		case CANTENWRITE:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(CANTENWRITE, HEX);
 		#else
  		Serial.println("Unable to Enable Writing to chip.");
  		Serial.println("Please make sure the HOLD & WRITEPROTECT pins are connected properly to VCC & GND respectively");
  		Serial.print("If you are still facing issues, ");
  		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 		#endif
 		break;

 		case PREVWRITTEN:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(PREVWRITTEN, HEX);
 		#else
  		Serial.println("This sector already contains data.");
  		Serial.println("Please make sure the sectors being written to are erased.");
  		Serial.print("If you are still facing issues, ");
  		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 		#endif
 		break;

 		case LOWRAM:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(LOWRAM, HEX);
 		#else
  		Serial.println("You are running low on SRAM. Please optimise your program for better RAM usage");
     #if defined (ARDUINO_ARCH_SAM)
     Serial.print("Current Free SRAM: ");
     Serial.println(_dueFreeRAM());
     #endif
   Serial.print("If you are still facing issues, ");
  		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 		#endif
 		break;

     case NOSUSPEND:
  		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x0");
 		Serial.println(NOSUSPEND, HEX);
 		#else
  		Serial.println("Unable to suspend operation.");
     Serial.print("If you are unable to resolve this problem, ");
  		Serial.println("please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 		#endif
 		break;

 		default:
 		#if defined (ARDUINO_ARCH_AVR) || defined (__AVR_ATtiny85__)
  		Serial.print("Error code: 0x");
 		Serial.println(UNKNOWNERROR, HEX);
 		#else
 		Serial.println("Unknown error");
  		Serial.println("Please raise an issue at http://www.github.com/Marzogh/SPIFlash/issues with the details of what your were doing when this error occurred");
 		#endif
 		break;
 	}
 }
 #endif
