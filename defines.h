/* Arduino SPIFlash Library v.2.3.1
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 02/06/2016
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

#define	MANID		 0x90
#define PAGEPROG     0x02
#define READDATA     0x03
#define FASTREAD	 0x0B
#define WRITEDISABLE 0x04
#define READSTAT1    0x05
#define READSTAT2	 0x35
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
#define BLOCK64ERASE 0xD8

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//					Chip specific instructions 						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

	//~~~~~~~~~~~~~~~~~~~~~~~~~ Winbond ~~~~~~~~~~~~~~~~~~~~~~~~~//
#define _MANID_WB	 0xEF
#define PAGESIZE_WB	 0x100


	//~~~~~~~~~~~~~~~~~~~~~~~~ Microchip ~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//							Definitions 							  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define BUSY         0x01
#define WRTEN        0x02
#define SUS 		 0x40
#define DUMMYBYTE	 0xEE

#define arrayLen(x)  	(sizeof(x) / sizeof(*x))
#define lengthOf(x)  	(sizeof(x))/sizeof(byte)
#define maxAddress		capacity
#define NO_CONTINUE		0x00
#define PASS			0x01
#define FAIL			0x00

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     					   List of Error codes						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 #ifdef RUNDIAGNOSTIC

 #define SUCCESS 	 	0x00
 #define CALLBEGIN		0x01
 #define UNKNOWNCHIP	0x02
 #define UNKNOWNCAP		0x03
 #define CHIPBUSY		0x04
 #define OUTOFBOUNDS	0x05
 #define CANTENWRITE	0x06
 #define PREVWRITTEN 	0x07
 #define UNKNOWNERROR	0xFF

 #endif
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
