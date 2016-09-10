/* Arduino SPIFlash Library v.2.4.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 11/09/2016
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//						Common Instructions 						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/*#define	MANID        0x90
#define PAGEPROG     0x02
#define READDATA     0x03
#define FASTREAD     0x0B
#define WRITEDISABLE 0x04
#define READSTAT1    0x05
#define READSTAT2    0x35
#define WRITESTAT    0x01
#define WRITEENABLE  0x06
#define SECTORERASE  0x20
#define BLOCK32ERASE 0x52
#define CHIPERASE    0xC7
#define SUSPEND      0x75
#define ID           0x90
#define RESUME       0x7A
#define JEDECID      0x9f
#define RELEASE      0xAB
#define POWERDOWN    0xB9
#define BLOCK64ERASE 0xD8*/

const uint8_t	MANID = 0x90;
const uint8_t PAGEPROG = 0x02;
const uint8_t READDATA = 0x03;
const uint8_t FASTREAD = 0x0B;
const uint8_t WRITEDISABLE = 0x04;
const uint8_t READSTAT1 = 0x05;
const uint8_t READSTAT2 = 0x35;
const uint8_t WRITESTAT = 0x01;
const uint8_t WRITEENABLE = 0x06;
const uint8_t SECTORERASE = 0x20;
const uint8_t BLOCK32ERASE = 0x52;
const uint8_t CHIPERASE = 0xC7;
const uint8_t SUSPEND = 0x75;
const uint8_t ID = 0x90;
const uint8_t RESUME = 0x7A;
const uint8_t JEDECID = 0x9F;
const uint8_t RELEASE = 0xAB;
const uint8_t POWERDOWN = 0xB9;
const uint8_t BLOCK64ERASE = 0xD8;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//					Chip specific instructions 						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

	//~~~~~~~~~~~~~~~~~~~~~~~~~ Winbond ~~~~~~~~~~~~~~~~~~~~~~~~~//
  #define WINBOND_MANID		 0xEF
  #define PAGESIZE	 0x100


	//~~~~~~~~~~~~~~~~~~~~~~~~ Microchip ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define MICROCHIP_MANID		 0xBF
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//							Definitions 							  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define BUSY        0x01
const uint8_t WRTEN = 0x02;
const uint8_t SUS = 0x40;
const uint8_t DUMMYBYTE = 0xEE;
const uint8_t NULLBYTE = 0x00;

#define arrayLen(x)  	(sizeof(x) / sizeof(*x))
#define lengthOf(x)  	(sizeof(x))/sizeof(byte)
#define maxAddress		capacity
#define NO_CONTINUE		0x00
#define PASS			0x01
#define FAIL			0x00

#if defined (ARDUINO_ARCH_ESP8266)
#define CS 15
#else
#define CS SS
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     					   List of Error codes						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

 #define SUCCESS 	 	0x00
 #define CALLBEGIN		0x01
 #define UNKNOWNCHIP	0x02
 #define UNKNOWNCAP		0x03
 #define CHIPBUSY		0x04
 #define OUTOFBOUNDS	0x05
 #define CANTENWRITE	0x06
 #define PREVWRITTEN 	0x07
 #define UNKNOWNERROR	0xFF

 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
