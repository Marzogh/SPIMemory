# SPIMemory [![Build Status](https://travis-ci.org/Marzogh/SPIMemory.svg?branch=master)](https://travis-ci.org/Marzogh/SPIMemory) [![DOI](https://zenodo.org/badge/35823047.svg)](https://zenodo.org/badge/latestdoi/35823047)
[![GitHub release](https://img.shields.io/github/release/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory)
[![GitHub commits](https://img.shields.io/github/commits-since/Marzogh/SPIMemory/v3.3.0.svg)](https://github.com/Marzogh/SPIMemory/compare/v3.3.0...v3.4.0)
[![GitHub issues](https://img.shields.io/github/issues/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory/pulls)
[![license](https://img.shields.io/github/license/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory/blob/master/LICENSE)
[![Join the chat at https://gitter.im/SPIMemory/community](https://badges.gitter.im/SPIMemory/community.svg)](https://gitter.im/SPIMemory/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Arduino library for Flash & FRAM Memory Chips (SPI based only)
<sup> Download the latest stable release from [here](https://github.com/Marzogh/SPIMemory/releases/latest). Please report any bugs in [issues](https://github.com/Marzogh/SPIMemory/issues/new).</sup>

This Arduino library is for use with flash and FRAM memory chips that communicate using the SPI protocol. In its current form it supports identifying the flash/FRAM chip and its various features; automatic address allocation and management; writing and reading a number of different types of data, ranging from 8-bit to 32-bit (signed and unsigned) values, floats, Strings, arrays of bytes/chars and structs to and from various locations; sector, block and chip erase; and powering down for low power operation.

<hr>

### IDE Compatibility (actually tested with)

- Arduino IDE v1.5.x
- Arduino IDE v1.6.x
- Arduino IDE v1.8.x

<hr>

### Platform compatibility

| Micro controllers | Dev boards tested with | Notes |
| ----------------- | ---------------------- | ----- |
| ATmega328P | Arduino Uno, Arduino Micro, <br> Arduino Fio, Arduino Nano | - |
| ATmega32u4 | Arduino Leonardo, Arduino Fio v3 | - |
| ATmega2560 | Arduino Mega | - |
| ATSAMD21G18 (ARM Cortex M0+) | Adafruit Feather M0, <br> Adafruit Feather M0 Express, <br> Adafruit ItsyBitsy M0 Express | - |
| AT91SAM3X8E (ARM Cortex M3) | Arduino Due | - |
| nRF52832 (ARM Cortex M4F) | Adafruit nRF52 Feather | - |
| ATSAMD51J19 (ARM Cortex M4) | Adafruit Metro M4 | - |
| STM32F091RCT6 | Nucleo-F091RC | |
| STM32L0 | Nucleo-L031K6 | |
| ESP8266 | Adafruit ESP8266 Feather, <br> Sparkfun ESP8266 Thing | - |
| ESP32 | Adafruit ESP32 Feather, <br> Sparkfun ESP32 Thing | Onboard flash memory. Refer to footnote<sup>£</sup> below. |
| Simblee | Sparkfun Simblee | - |

<sup>£ </sup><sub> ESP32 boards usually have an SPI Flash already attached to their default SS pin, so the user has to explicitly declare the `ChipSelect` pin being used with the constructor</sub>

<hr>

### Flash memory compatibility - Actually tested with

| Manufacturer | Flash IC | Notes |
| ------------ | -------- | ----- |
| Winbond | W25Q16BV <br> W25Q64FV <br> W25Q64JV <br> W25Q80BV <br> W25Q256FV | Should work with the W25QXXXBV, W25QXXXFV & <br> W25QXXXJV families |
| Microchip | SST25VF064C <br> SST26VF016B <br> SST26VF032B <br> SST26VF064B | Should work with the SST25 & SST26 families |
| Cypress/Spansion | S25FL032P <br> S25FL116K <br> S25FL127S | Should work with the S25FL family |
| ON Semiconductor | LE25U40CMC |  |
| AMIC| A25L512A0 |  |
| Micron| M25P40 |  |
| Adesto| AT25SF041 |  |
| Macronix| MX25L4005 <br> MX25L4005 |  |
| Giga devices| GD25Q16C | (Used on the Adafruit ItsyBitsy M0 Express) |


##### Should work with any flash memory that is compatible with the SFDP standard as defined in JESD216B

<hr>

### FRAM memory compatibility - Actually tested with

| Manufacturer | Flash IC | Notes |
| ------------ | -------- | ----- |
| Cypress/Spansion | FM25W256 | Should work with the FM25W family |

<hr>

### Installation

#### Option 1
- Open the Arduino IDE.
- Go to Sketch > Include Library > Manage libraries.
- Search for SPIMemory.
- Install the latest version.

#### Option 2
- Click on the 'Clone or download' button above the list of files on this <a href = "https://github.com/Marzogh/SPIMemory/tree/master"> page </a>.
- Select Download ZIP. A .zip file will download to your computer.
- Unzip the archive and rename resulting folder to 'SPIMemory'
- Move the folder to your libraries folder (~/sketches/libraries)

### Usage

#### SPIFlash

- The library is called by declaring the```SPIFlash flash(csPin*)``` constructor where 'flash' can be replaced by a user constructor of choice and 'csPin' is the Chip Select pin for the flash module.

    <sub>* Optional. Do not include csPin if using the default slave select pin for your board.</sub>
- Every version of the library >= v3.0.0 supports the ability to use any of multiple SPI interfaces (if your micro-controller supports them). Switching to use another SPI interface is done by calling ```SPIFlash flash(csPin, &SPI1);``` (or &SPI2 and so on), instead of ```SPIFlash flash(csPin)```.

    <sub>* NOTE: This is currently only supported on the SAMD and STM32 architectures.</sub>
- An alternate version ```SPIFlash flash (SPIPinsArray)``` of the constructor can be used (only with ESP32 board as of now) to enable the use of custom SPI pins. ```SPIPinsArray``` has to be a 4 element array containing the custom SPI pin numbers (as signed integers - int8_t) in the following order - sck, miso, mosi, ss.
- Also make sure to include ```flash.begin(CHIPSIZE*)``` in ```void setup()```. This enables the library to detect the type of flash chip installed and load the right parameters.

    <sub>* Optional</sub>

##### Note on SFDP discovery
As of v3.2.1, SFDP parameter discovery is an user controlled option. To get the library to work with SFDP compatible flash memory chips that are not officially supported by the library, the user must uncomment '//#define USES_SFDP' in 'SPIMemory.h'.

##### Notes on Address overflow and Error checking
- The library has Address overflow enabled by default - i.e. if the last address read/written from/to,  in any function, is 0xFFFFF then, the next address read/written from/to is 0x00000. This can be disabled by uncommenting ```#define DISABLEOVERFLOW``` in SPIMemory.h. (Address overflow only works for Read / Write functions. Erase functions erase only a set number of blocks/sectors irrespective of overflow.)


- All write functions have Error checking turned on by default - i.e. every byte written to the flash memory will be checked against the data stored on the Arduino. Users who require greater write speeds can disable this function by setting an optional last 'errorCheck' argument in any write function to NOERRCHK - For eg. call the function ```writeByte(address, *data_buffer, NOERRCHK)``` instead of ```writeByte(address, *data_buffer)```.

#### SPIFram <sup>^</sup>

- The library is called by declaring the```SPIFram fram(csPin*)``` constructor where 'fram' can be replaced by a user constructor of choice and 'csPin' is the Chip Select pin for the fram module.

    <sub>* Optional. Do not include csPin if using the default slave select pin for your board.</sub>
- Every version of the library >= v3.0.0 supports the ability to use any of multiple SPI interfaces (if your micro-controller supports them). Switching to use another SPI interface is done by calling ```SPIFram fram(csPin, &SPI1);``` (or &SPI2 and so on), instead of ```SPIFram fram(csPin)```.

    <sub>* NOTE: This is currently only officially supported on the SAMD and STM32 architectures.</sub>
- Also make sure to include ```fram.begin(CHIPSIZE*)``` in ```void setup()```. This enables the library to detect the type of fram chip installed and load the right parameters.

<sup>^</sup> <sub> Currently in BETA. The methods in SPIFram are not final and subject to change over the next few revisions.</sub>

The library enables the following functions:
<hr>

#### Non-Read/Write functions
<hr>

###### `begin(_chipsize)`

###### `setClock(clockSpeed)`

###### `error(_verbosity)`

###### `getManID()`

###### `getJEDECID()`

###### `getUniqueID()`

###### `getAddress(sizeOfData)`

###### `sizeofStr()`

###### `getCapacity()`

###### `getMaxPage()`

###### `functionRunTime()`

<hr>

##### Read commands
<hr>
<sub>
All read commands take a last boolean argument 'fastRead'. This argument defaults to FALSE, but when set to TRUE carries out the Fast Read instruction so data can be read at up to the memory's maximum frequency.
</sub>

<sub>
All read commands only take a 32-bit address variable instead of the optional 16-bit page number & 8-bit offset variables in previous versions of the library (< v3.0.0)
</sub>

###### `readAnything(address, value)`

###### `readByte(address)`

###### `readChar(address)`

###### `readWord(address)`

###### `readShort(address)`

###### `readULong(address)`

###### `readLong(address)`

###### `readFloat(address)`

###### `readStr(address, outputStr)`

###### `readAnything(address, value)`
<hr>

##### Write commands
<hr>
<sub>
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.
</sub>

<sub> All write commands only take a 32-bit address variable instead of the optional 16-bit page number & 8-bit offset variables in previous versions of the library (< v3.0.0) </sub>

###### `writeByte(address, data)`

###### `writeChar(address, data)`

###### `writeWord(address, data)`

###### `writeShort(address, data)`

###### `writeULong(address, data)`

###### `writeLong(address, data)`

###### `writeFloat(address, data)`

###### `writeStr(address, data)`

###### `writeAnything(address, value)`

<hr>

##### Continuous read/write commands
<sub>
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.
</sub>
<hr>

###### `readByteArray(address, *data_buffer, bufferSize)`

###### `writeByteArray(address, *data_buffer, bufferSize)`

###### `readCharArray(address, *data_buffer, bufferSize)`

###### `writeCharArray(address, *data_buffer, bufferSize)`

<hr>

##### Erase commands
<sub>
All erase commands only take a 32-bit address variable instead of the optional 16-bit page number & 8-bit offset variables in previous versions of the library (< v3.0.0)
</sub>
<hr>

###### `eraseSector(address)`

###### `eraseBlock32K(address)`

###### `eraseBlock64K(address)`

###### `eraseChip()`

###### `eraseSection(address, sizeOfData)`

<hr>

##### Suspend/Resume commands
<hr>

###### `suspendProg()`

###### `resumeProg()`
<hr>

##### Power operation commands
<hr>

###### `powerDown()`

###### `powerUp()`

<hr>

##### Error codes explained

Note: If you are unable to fix the error/s, please raise an issue [here](http://www.github.com/Marzogh/SPIMemory/issues) with the details of your flash chip and what you were doing when this error occurred. **Please follow the issue template that show up at the link**

| Error code  | Explanation |
| :-----------: | ------------- |
| **0x00** | Function executed successfully |
| **0x01** | `*constructor_of_choice*.begin()` was not called in `void setup()` |
| **0x02** | Unable to identify chip. <br> Is this chip officially supported? |
| **0x03** | Unable to identify capacity. <br> Is this chip officially supported? <br> If not, please define a `CAPACITY` constant and include it in `flash.begin(CAPACITY)`. |
| **0x04** | Chip is busy. Make sure all pins have been connected properly. |
| **0x05** | Page overflow has been disabled and the address called exceeds the memory |
| **0x06** | Unable to Enable Writing to chip. <br> Please make sure the HOLD & WRITEPROTECT pins are pulled up to VCC. |
| **0x07** | This sector already contains data. <br> Please make sure the sectors being written to are erased. |
| **0x08** | You are running low on RAM. <br> Please optimise your code for better RAM usage |
| **0x09** | Unable to suspend/resume operation. <br> Please raise an issue. |
| **0x0A** | Write Function has failed errorcheck. <br> Please raise an issue. |
| **0x0B** | Check your wiring. Flash chip is non-responsive. |
| **0x0C** | This function is not supported by the memory hardware. |
| **0x0D** | Unable to enable 4-byte addressing. <br> Please raise an issue. |
| **0x0E** | Unable to disable 4-byte addressing. <br> Please raise an issue. |
| **0x0F** | The chip is currently powered down. |
| **0x10** | The Flash chip does not support SFDP. |
| **0x11** | Unable to read Erase Parameters from chip. <br> Reverting to library defaults. |
| **0x12** | Unable to read erase times from flash memory. <br> Reverting to library defaults. |
| **0x13** | Unable to read program times from flash memory. <br> Reverting to library defaults. |
| **0x14** | No Chip Select pin defined in the custom SPI Array. <br> Refer to section about Constructor for information on how to use custom SPI pins. |
| **0xFE** | Unknown error. <br> Please raise an issue. |

<hr>
