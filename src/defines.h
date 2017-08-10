/* Arduino SPIFlash Library v.3.0.0
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
#define WRITESTATEN  0x50
#define WRITESTAT    0x01
#define WRITEENABLE  0x06
#define SECTORERASE  0x20
#define BLOCK32ERASE 0x52
#define CHIPERASE    0x60
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
//            B = Bytes; KB = Kilo Bytes; MB = Mega Bytes               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*#define B1            1L * B
#define B2            2L * B
#define B4            4L * B
#define B8            8L * B
#define B16           16L * B
#define B32           32L * B
#define B64           64L * B
#define B80           80L * B
#define B128          128L * B
#define B256          256L * B
#define B512          512L * B
#define KB1           1L * K
#define KB2           2L * K
#define KB4           4L * K
#define KB8           8L * K
#define KB16          16L * K
#define KB32          32L * K
#define KB64          64L * K
#define KB128         128L * K
#define KB256         256L * K
#define KB512         512L * K
#define MB1           1L * M
#define MB2           2L * M
#define MB4           4L * M
#define MB8           8L * M
#define MB16          16L * M
#define MB32          32L * M
#define MB64          64L * M
#define MB128         128L * M
#define MB256         256L * M
#define MB512         512L * M*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//					Chip specific instructions 						  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

	//~~~~~~~~~~~~~~~~~~~~~~~~~ Winbond ~~~~~~~~~~~~~~~~~~~~~~~~~//
  #define WINBOND_MANID		 0xEF
  #define PAGESIZE	 0x100
  #define WINBOND_WRITE_DELAY 0x02
  #define WINBOND_WREN_TIMEOUT  10L
  //#define CHIPSIZE 1*M

	//~~~~~~~~~~~~~~~~~~~~~~~~ Microchip ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define MICROCHIP_MANID		 0xBF

  //~~~~~~~~~~~~~~~~~~~~~~~~ Cypress ~~~~~~~~~~~~~~~~~~~~~~~~//
  #define CYPRESS_MANID		 0x1
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//							Definitions 							  //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#define BUSY          0x01
#if defined (ARDUINO_ARCH_ESP32)
#define SPI_CLK       20000000
/*#elif defined (ARDUINO_ARCH_SAMD)
#define SPI_CLK       8000000*/
#else
#define SPI_CLK       104000000       //Hex equivalent of 104MHz
//#define SPI_CLK       4000000       //Hex equivalent of 104MHz
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
#define PRINTOVERRIDE true
#define ERASEFUNC     0xEF
#if defined (SIMBLEE)
#define BUSY_TIMEOUT  100L
#else
#define BUSY_TIMEOUT  1000L
#endif
#define arrayLen(x)   (sizeof(x) / sizeof(*x))
#define lengthOf(x)   (sizeof(x))/sizeof(byte)
#define B             1L
#define K             1024L
#define M             K * K
#define S             1000L

#if defined (ARDUINO_ARCH_ESP8266)
#define CS 15
#elif defined (ARDUINO_ARCH_SAMD)
#define CS 10
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
//                        Bit shift macros                            //
//                      Thanks to @VitorBoss                          //
//          https://github.com/Marzogh/SPIFlash/issues/76             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#define Lo(param) ((char *)&param)[0] //0x000y
#define Hi(param) ((char *)&param)[1] //0x00y0
#define Higher(param) ((char *)&param)[2] //0x0y00
#define Highest(param) ((char *)&param)[3] //0xy000
#define Low(param) ((int *)&param)[0] //0x00yy
#define Top(param) ((int *)&param)[1] //0xyy00
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
