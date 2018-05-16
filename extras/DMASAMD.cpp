/* Arduino SPIMemory Library v.2.6.0
 * Copyright (C) 2017 by Prajwal Bhattaram
 * Created by Prajwal Bhattaram - 30/09/2016
 * Modified by Prajwal Bhattaram - 14/04/2017
 * Original code from @manitou48 <https://github.com/manitou48/Zero/blob/master/SPIdma.ino>
 *
 * This file is part of the Arduino SPIMemory Library. This library is for
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
 * along with the Arduino SPIMemory Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "SPIMemory.h"

//--------------------------- Private Arduino Zero Variables ----------------------------//
  Sercom *sercom = (Sercom   *)ZERO_SPISERCOM;  //Set SPI SERCOM
  // DMA   12 channels
  typedef struct {
    uint16_t btctrl;
    uint16_t btcnt;
    uint32_t srcaddr;
    uint32_t dstaddr;
    uint32_t descaddr;
  } dmacdescriptor ;
  volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));
  dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));
  dmacdescriptor descriptor __attribute__ ((aligned (16)));

  static uint32_t chnltx = 0, chnlrx = 1; // DMA channels
  enum XfrType { DoTX, DoRX, DoTXRX};
  static XfrType xtype;
  static uint8_t rxsink[1], txsrc[1] = {0xff};
  volatile uint32_t dmadone;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//        Private functions used by Arduino Zero DMA operations        //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SPIMemory::_zeroDMAC_Handler(void) {
  // interrupts DMAC_CHINTENCLR_TERR DMAC_CHINTENCLR_TCMPL DMAC_CHINTENCLR_SUSP
  uint8_t active_channel;

  // disable irqs ?
  __disable_irq();
  active_channel =  DMAC->INTPEND.reg & DMAC_INTPEND_ID_Msk; // get channel number
  DMAC->CHID.reg = DMAC_CHID_ID(active_channel);
  dmadone = DMAC->CHINTFLAG.reg;
  DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL; // clear
  DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
  DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
  __enable_irq();
}

void SPIMemory::_zeroDma_init() {
  // probably on by default
  PM->AHBMASK.reg |= PM_AHBMASK_DMAC ;
  PM->APBBMASK.reg |= PM_APBBMASK_DMAC ;
  NVIC_EnableIRQ( DMAC_IRQn ) ;

  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
  DMAC->WRBADDR.reg = (uint32_t)wrb;
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
}

void SPIMemory::_zeroSpi_xfr(void *txdata, void *rxdata,  size_t n) {
  uint32_t temp_CHCTRLB_reg;

  // set up transmit channel
  DMAC->CHID.reg = DMAC_CHID_ID(chnltx);
  DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
  DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
  DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnltx));
  temp_CHCTRLB_reg = DMAC_CHCTRLB_LVL(0) |
    DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) | DMAC_CHCTRLB_TRIGACT_BEAT;
  DMAC->CHCTRLB.reg = temp_CHCTRLB_reg;
  DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ; // enable all 3 interrupts
  descriptor.descaddr = 0;
  descriptor.dstaddr = (uint32_t) &sercom->SPI.DATA.reg;
  descriptor.btcnt =  n;
  descriptor.srcaddr = (uint32_t)txdata;
  descriptor.btctrl =  DMAC_BTCTRL_VALID;
  if (xtype != DoRX) {
    descriptor.srcaddr += n;
    descriptor.btctrl |= DMAC_BTCTRL_SRCINC;
  }
  memcpy(&descriptor_section[chnltx],&descriptor, sizeof(dmacdescriptor));

  // rx channel    enable interrupts
  DMAC->CHID.reg = DMAC_CHID_ID(chnlrx);
  DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
  DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
  DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnlrx));
  temp_CHCTRLB_reg = DMAC_CHCTRLB_LVL(0) |
    DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_RX) | DMAC_CHCTRLB_TRIGACT_BEAT;
  DMAC->CHCTRLB.reg = temp_CHCTRLB_reg;
  DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ; // enable all 3 interrupts
  dmadone = 0;
  descriptor.descaddr = 0;
  descriptor.srcaddr = (uint32_t) &sercom->SPI.DATA.reg;
  descriptor.btcnt =  n;
  descriptor.dstaddr = (uint32_t)rxdata;
  descriptor.btctrl =  DMAC_BTCTRL_VALID;
  if (xtype != DoTX) {
    descriptor.dstaddr += n;
    descriptor.btctrl |= DMAC_BTCTRL_DSTINC;
  }
  memcpy(&descriptor_section[chnlrx],&descriptor, sizeof(dmacdescriptor));

  // start both channels  ? order matter ?
  DMAC->CHID.reg = DMAC_CHID_ID(chnltx);
    Serial.println("A");
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
  system_interrupt_leave_critical_section();
    Serial.println("B");
  DMAC->CHID.reg = DMAC_CHID_ID(chnlrx);
    Serial.println("C");
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
    Serial.println("D");

  while(!dmadone);  // await DMA done isr

  DMAC->CHID.reg = DMAC_CHID_ID(chnltx);   //disable DMA to allow lib SPI
  DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
  DMAC->CHID.reg = DMAC_CHID_ID(chnlrx);
  DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
}

void SPIMemory::_zeroSpi_write(void *data,  size_t n) {
  xtype = DoTX;
  Serial.println("SPI_write started");
  _zeroSpi_xfr(data,rxsink,n);
  Serial.println("WRITE DONE");
}
void SPIMemory::_zeroSpi_read(void *data,  size_t n) {
  xtype = DoRX;
  _zeroSpi_xfr(txsrc,data,n);
  Serial.println("READ DONE");
}
void SPIMemory::_zeroSpi_transfer(void *txdata, void *rxdata,  size_t n) {
  xtype = DoTXRX;
  _zeroSpi_xfr(txdata,rxdata,n);
  Serial.println("transfer done");
}
