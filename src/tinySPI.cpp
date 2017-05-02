/* Arduino SPIFlash Library v.2.7.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 19/04/2017
 * Modified by Prajwal Bhattaram - 19/04/2017
 * Original code from @manitou48 <https://github.com/manitou48/DUEZoo/blob/master/dmaspi.ino>
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

#if defined (__AVR_ATtiny85__)
void SPIFlash::_tinySPIbegin() {
  PORTB &= ~(_BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | cs_mask);
  DDRB  &= ~_BV(PORTB0); // DI (NOT MISO)
  DDRB  |=  _BV(PORTB1)  // DO (NOT MOSI)
        |   _BV(PORTB2)  // SCK
        |   cs_mask;     // CS
}

static uint8_t SPIFlash::_tinySPItransfer(uint8_t _data) {
  USIDR = _data;
  for (uint8_t i = 0; i < 8; i++) {
    SPIBIT
  }
  /*SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT
  SPIBIT*/
  return USIDR;
}
#endif
