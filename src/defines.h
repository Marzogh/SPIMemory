/* Arduino SPIFlash Library v.2.7.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//						Common Instructions 						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define	MANID        0x90
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
#define JEDECID      0x9F
#define RELEASE      0xAB
#define POWERDOWN    0xB9
#define BLOCK64ERASE 0xD8
#define READSFDP     0x5A

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                     General size definitions                       //
//            B = Bytes; KB = Kilo bits; MB = Mega bits               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define B1            1L
#define B2            2L
#define B4            4L
#define B8            8L
#define B16           16L
#define B32           32L
#define B64           64L
#define B80           80L
#define B128          128L
#define B256          256L
#define B512          512L
#define KB1           B1 * K
#define KB2           B2 * K
#define KB4           B4 * K
#define KB8           B8 * K
#define KB16          B16 * K
#define KB32          B32 * K
#define KB64          B64 * K
#define KB128         B128 * K
#define KB256         B256 * K
#define KB512         B512 * K
#define MB1           B1 * M
#define MB2           B2 * M
#define MB4           B4 * M
#define MB8           B8 * M
#define MB16          B16 * M
#define MB32          B32 * M
#define MB64          B64 * M
#define MB128         B128 * M
#define MB256         B256 * M
#define MB512         B512 * M
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

#define BUSY          0x01
#if defined (ARDUINO_ARCH_ESP32)
#define SPI_CLK       20000000
#else
#define SPI_CLK       104000000       //Hex equivalent of 104MHz
#endif
#define WRTEN         0x02
#define SUS           0x80
#define WSE           0x04
#define WSP           0x08
#define DUMMYBYTE     0xEE
#define NULLBYTE      0x00
#define NULLINT       0x0000
#define NO_CONTINUE   0x00
#define PASS          0x01
#define FAIL          0x00
#define NOOVERFLOW    false
#define NOERRCHK      false
#define VERBOSE       true
#if defined (SIMBLEE)
#define BUSY_TIMEOUT  100L
#else
#define BUSY_TIMEOUT  10L
#endif
#define arrayLen(x)   (sizeof(x) / sizeof(*x))
#define lengthOf(x)   (sizeof(x))/sizeof(byte)
#define K             1024L
#define M             K * K
#define S             1000L

#if defined (ARDUINO_ARCH_ESP8266)
#define CS 15
#elif defined (ARDUINO_ARCH_SAMD)
#define CS 10
#elif defined __AVR_ATtiny85__
#define CS 5
/*********************************************************************************************
// Declaration of the Default Chip select pin name for RTL8195A
// Note: This has been shifted due to a bug identified in the HAL layer SPI driver
// @ref http://www.amebaiot.com/en/questions/forum/facing-issues-with-spi-interface-to-w25q32/
// Note: Please use any pin other than GPIOC_0 which is the D10 marked in the kit
// Original edit by @boseji <salearj@hotmail.com> 02.03.17
// Modified by Prajwal Bhattaram <marzogh@icloud.com> 14.4.17
**********************************************************************************************/
#elif defined (BOARD_RTL8195A)
#define CS PC_4
#else
#define CS SS
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//                    Arduino Due DMA definitions                     //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Use SAM3X DMAC if nonzero
#define USE_SAM3X_DMAC 1
// Use extra Bus Matrix arbitration fix if nonzero
#define USE_SAM3X_BUS_MATRIX_FIX 0
// Time in ms for DMA receive timeout
#define SAM3X_DMA_TIMEOUT 100
// chip select register number
#define SPI_CHIP_SEL 3
// DMAC receive channel
#define SPI_DMAC_RX_CH  1
// DMAC transmit channel
#define SPI_DMAC_TX_CH  0
// DMAC Channel HW Interface Number for SPI TX.
#define SPI_TX_IDX  1
// DMAC Channel HW Interface Number for SPI RX.
#define SPI_RX_IDX  2
// Set DUE SPI clock div (any integer from 2 - 255)
#define DUE_SPI_CLK 2
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//     					   List of Error codes						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

 #define SUCCESS      0x00
 #define CALLBEGIN    0x01
 #define UNKNOWNCHIP  0x02
 #define UNKNOWNCAP   0x03
 #define CHIPBUSY     0x04
 #define OUTOFBOUNDS  0x05
 #define CANTENWRITE  0x06
 #define PREVWRITTEN  0x07
 #define LOWRAM       0x08
 #define SYSSUSPEND   0x09
 #define UNSUPPORTED  0x0A
 #define ERRORCHKFAIL 0x0B
 #define NORESPONSE   0x0C
 #define UNKNOWNERROR 0xFE

 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
