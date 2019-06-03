/* Arduino SPIMemory Library v.3.4.0
 * Copyright (C) 2019 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/05/2015
 * Modified by Prajwal Bhattaram - 02/06/2019
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

 #ifndef DIAGNOSTICS_H
 #define DIAGNOSTICS_H

 #include "SPIMemory.h"
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //                       List of Error codes                          //
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  #define SUCCESS                 0x00
  #define CALLBEGIN               0x01
  #define UNKNOWNCHIP             0x02
  #define UNKNOWNCAP              0x03
  #define CHIPBUSY                0x04
  #define OUTOFBOUNDS             0x05
  #define CANTENWRITE             0x06
  #define PREVWRITTEN             0x07
  #define LOWRAM                  0x08
  #define SYSSUSPEND              0x09
  #define ERRORCHKFAIL            0x0A
  #define NORESPONSE              0x0B
  #define UNSUPPORTEDFUNC         0x0C
  #define UNABLETO4BYTE           0x0D
  #define UNABLETO3BYTE           0x0E
  #define CHIPISPOWEREDDOWN       0x0F
  #define NOSFDP                  0x10
  #define NOSFDPERASEPARAM        0x11
  #define NOSFDPERASETIME         0x12
  #define NOSFDPPROGRAMTIMEPARAM  0x13
  #define NOCHIPSELECTDECLARED    0x14
  #define UNKNOWNERROR            0xFE
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

 class Diagnostics {
 public:
   //------------------------------------ Constructor ------------------------------------//

  Diagnostics(void){};
  ~Diagnostics(void){};
   //------------------------------- Public functions -----------------------------------//
   void     troubleshoot(uint8_t _code, bool printoverride = false);

   uint8_t errorcode;
 private:
   void     _printErrorCode(void);
   void     _printSupportLink(void);

 };

 extern Diagnostics diagnostics; ///< default Diagnostics instance

 #endif
