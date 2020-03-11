# SPIMemory
##### Change log
[![Build Status](https://travis-ci.org/Marzogh/SPIMemory.svg?branch=master)](https://travis-ci.org/Marzogh/SPIMemory) [![DOI](https://zenodo.org/badge/35823047.svg)](https://zenodo.org/badge/latestdoi/35823047)
[![GitHub release](https://img.shields.io/github/release/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory)
[![GitHub commits](https://img.shields.io/github/commits-since/Marzogh/SPIMemory/v3.4.0.svg)](https://github.com/Marzogh/SPIMemory/compare/v3.3.0...v3.4.0)
[![GitHub issues](https://img.shields.io/github/issues/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory/pulls)
[![license](https://img.shields.io/github/license/Marzogh/SPIMemory.svg)](https://github.com/Marzogh/SPIMemory/blob/master/LICENSE)
[![Join the chat at https://gitter.im/SPIMemory/community](https://badges.gitter.im/SPIMemory/community.svg)](https://gitter.im/SPIMemory/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
>This Arduino library is for use with flash and FRAM memory chips that communicate using the SPI protocol. In its current form it supports identifying the flash/FRAM chip and its various features; automatic address allocation and management; writing and reading a number of different types of data, ranging from 8-bit to 32-bit (signed and unsigned) values, floats, Strings, arrays of bytes/chars and structs to and from various locations; sector, block and chip erase; and powering down for low power operation.

<hr>

## Version 3.4.0

#### Release date 03.06.2019

###### Bugs squashed:
- FastRead works properly now. All previous versions of the library has a FastRead bug that prevent it from doing what it was supposed to.

###### Enhancements:
- An alternate version of the constructor
```
SPIFlash flash(int8_t *SPIPinsArray);
```
can be used (only with ESP32 boards as of now) to enable the use of custom SPI pins. ```SPIPinsArray``` has to be a 4 element array containing the custom SPI pin numbers (as signed integers - int8_t) in the following order - sck, miso, mosi, ss. Refer to the wiki for more information. Resolves #113
- Created new error code `NOCHIPSELECTDECLARED` for errors with declaring custom SPI pins.
- Using other SPI ports (HSPI, VSPI, SPI1, SPI2 etc.) is now also supported on ESP32 boards - along with SAMD and STM32 boards (supported since v3.0.0). Resolves #177
- Formatted and merged pull request #153. This changes the way [setClock()](https://github.com/Marzogh/SPIMemory/wiki/Library-instantiation-functions#setclockclockspeed) works and allows for the definition of clock speed before the SPI Bus is instantiated. Refer to [wiki](https://github.com/Marzogh/SPIMemory/wiki/Library-instantiation-functions#setclockclockspeed) for further details. Thanks @rambo.

###### New Microcontrollers supported:
- nRF52832 ARM Cortex M4F (Adafruit nRF52 Feather)
- STM32L0 ARM Cortex M0+ (Nucleo-L031K6)

###### New flash memory chips supported:
- MX25L4005 & MX25L4005 from Macronix


<hr>

## Version 3.3.0

#### Release date 20.04.2019

>Library now supports FRAM memory (In BETA). Documentation for this is in progress.

###### Bugs squashed:
- Fixed a major bug causing issues with page boundaries in writeAnything and writeStr. Thanks for your help @jacky4566 (Issue #151)
- Fixed a major bug with reading Strings (Issue #143)
- @incorvia fixed a bug by initialising the SPIBusState to false
- Fixes bug that causes ballooning of pagesize when SFDP is not read. This is because no default `_pageSize` value was set in this scenario. This has now been fixed
- Fixed a major bug in how SFDP data is used to calculate chip capacity. The previous version of the library returned a wrong value. This has now been fixed. Refer to comments in `_getSFDPFlashParam(void)` for further details.

###### Enhancements:
- Reduce SRAM footprint when running Diagnostics.ino. Thanks @rambo (PR #157)
- Officially supports the ESP32 (tested on the ESP32 Feather)
- Now supports FRAM memory chips via the `SPIFram` constructor
- Changed Diagnostics output in `_chipID()` to match situation better.
- Fixed `_chipID()` to run more efficiently.

###### New flash memory chips supported:
- GD25Q16C from Giga devices (Used on the Adafruit ItsyBitsy M0 Express)
- W25Q64JV from Winbond
- SST26VF016B & SST26VF032B from Microchip. (Thanks @mix86)

###### New FRAM memory chips supported:
- FM25W256 from Cypress

<hr>

## Version 3.2.1

#### Release date 13.04.2018

###### Bugs squashed
- Fixes issue #135 : The addition of the SFDP checking to `_chipID()` resulted in a sudden (very large) increase in compiled code size. As of the current version (v3.2.1), SFDP checking is an user controlled option. To get the library to work with SFDP compatible flash memory that is not officially supported, the user must uncomment `//#define USES_SFDP` in `SPIMemory.h`.

- Moved `bool _loopedOver` from being a local variable in `getAddress()` to a global one. Now it actually does what it was meant to do - i.e. prevent looping over data a second time.

###### Enhancements
- The way the basic functions execute has been modified to keep the function runtime the same (or improved) while reducing their memory footprint.

| Test sketch  | Test platform |
|:-----:|:-----:|
|FlashDiagnostics.ino from v3.2.0<br>with #RUNDIAGNOSTIC commented out | Arduino Pro Mini 8MHz 3.3V |


| Library version | Compiled code size<br>(Bytes) | SFDP status | Size compared to v3.1.0 |
| :------: | :------: | :------: | :-----: |
| v3.1.0 | 17652 | SFDP discovery not supported | Same size |
| v3.2.0 | 20104 | SFDP discovery supported & on by default | 13.9% larger than v3.1.0 |
| v3.2.1 | 15316 | SFDP discovery turned off | 13.75% smaller than v3.1.0 |
| v3.2.1 | 17854 | SFDP discovery turned on | 1.1% larger than v3.1.0 |

<hr>

## Version 3.2.0

#### Release date 24.01.2018

- Library name changed to SPIMemory

- The library now gathers sfdp information through `_chipID()`. SFDP information is collected when available, otherwise the library defaults to the pre v3.2.0 version.

###### New flash memory chips supported
- All flash memories compatible with the SFDP standard (as at JESD216B) should be compatible with the library

- LE25U40CMC from ON Semiconductor
- A25L512A0 from AMIC
- M25P40 from Micron
- AT25SF041 from Adesto
- S25FL032P from Cypress/Spansion

###### New Microcontrollers supported
- ATSAMD51J19 ARM Cortex M4 (Adafruit Metro M4)

###### Enhancements
The library now does the following - if the flash memory chip is compatible with the SFDP standard:
- Identifies the flash memory chip using the Serial Flash Discoverable Parameters standard. (JESD216B)
- Identifies the following opcodes from the SFDP tables and uses them to read/write data:
	- `eraseSector`
	- `eraseBlock32K`
	- `eraseBlock64K`
- Reads timing information from SFDP to accurately time the following operations:
	- `eraseSector`
	- `eraseBlock32K`
	- `eraseBlock64K`
- Fixes #125 in commit 10c2b86. The library automatically uses the alternate chip erase code (0xC7) as required (only on fully supported chips)

<hr>

## Version 3.1.0

#### Release date 24.01.2018

>Library name change notice inserted into `flash.begin()`. This notice can be dismissed by commenting out the instance of `#define PRINTNAMECHANGEALERT` in SPIFlash.h. Please refer to the Readme file for further details.

Bugs squashed
- An error with how `_addressCheck()` works with data that spans the memory boundary - when rolling over from address `_chip.capacity` to address `0x00` - has been fixed. In previous versions this caused issues with writing complex data structures across the memory boundary and led to many `_writeErrorCheck()` failures. Fixes issue #112

- An error with how `_writeErrorCheck()` worked has been resolved. Writing structs is now as stable as other functions are. Fixes #106

- All compilation-time, non-critical errors have been fixed.

###### Enhancements:
- A new function - `flash.eraseSection(address, size)` - has been introduced in this version. When a user requires a large and variable (between writes) amount of data to be written to the flash memory on the fly and to have the correct amount of space erased to fit the data, this function will automatically calculate and erase the right amount of space to fit the data. Please note that if the the amount of data being written is consistently the same size, the pre-existing `flash.eraseSector()`, `flash.eraseBlock32K()` and `flash.eraseBlock64K()` functions will operate a lot faster.
- Updated `Diagnostics.ino` to include `eraseSection()`.

- All I/O functions now check to see if the flash chip is powered down. If it is, they prevent the function from running and returns an error. A new error code 'CHIPISPOWEREDDOWN' will be returned upon calling `flash.error()`.


###### New flash memory chips supported:
- S25FL127S

<hr>

## Version 3.0.1

#### Release date 10.12.2017

###### Bugs squashed:

- Thanks for catching and fixing the following @hanyazou (PR #101)
	- Added missing break statements in `_beginSPI()` because that lacks some essential break statement in switch/case. (commit 1: 1de2c10)
	- The library did not wait until the busy bit in SR1 was cleared. Timeout argument of `_notBusy()` must be in micro-second. 500L means 500 micro seconds which is too short to wait for completion of erase command. Added " * 1000" to the arguments. (commit 2: 9633c2f)
	- Modified `TestFlash.ino` to check error of erase commands execution. (commit 3: a179306)

- Fixes a major bug that was causing input Arrays to be overwritten by zeros during the writeByteArray() / writeCharArray() process. Thanks for catching this @hanyazou. (Issue #102)

###### New flash memory chips supported:
- S25FL127S


<hr>

## Version 3.0.0

#### Release date 09.08.2017

###### Bugs Squashed:
- The `writeByteArray()` & `writeCharArray()` bug that occurred when writing arrays that spanned page boundaries (squashed in v2.5.0), stayed around to haunt the other functions. Writing any data larger than a single byte that spanned page boundaries would cause the data to wrap around to the beginning of the page. This has now been squashed in this release.

###### Deprecations:
- Going forward the ATTiny85 is no longer officially supported.
- The library no longer supports using the page number + offset combination instead of addresses. If your code requires you to use a page number + offset combination, use the following code to help
```
address = (pagenumber << 8) + offset.
(32 bit) |     (16 bit)     | (8 bit)
```
- The constructor no longer takes the pageOverflow variable as an argument. Page overflow is globally enabled by default and can be disabled by including a "#define DISABLEOVERFLOW" at the beginning of SPIFlash.h.

###### New Boards supported:
- Nucleo-F091RC
- Adafruit Feather M0

###### New Flash manufacturers supported:
- Microchip (SST25 & SST26 series)
- Cypress
- Spansion

###### Enhancements:
- Confirmed to work with SPANSION/CYPRESS & MICROCHIP (Both SST25 & SST26 series).
- If not using an officially supported chip, use the following variation of flash.begin() :
```
flash.begin(flashChipSize)
```

 where flashChipSize is indicated in Bytes, Kilobytes or Megabytes. (Refer to the next two items in this change log)

- Including 'flashChipSize' in flash.begin() compiles more efficiently than in previous versions.
- The way memory size is indicated by the users in flash.begin(flashChipSize) has changed - please refer to defines.h or the wiki for further information. The new method enables users to define any custom size unlike the previous version where only a limited number of predetermined sizes were available to use.
- Library faster than before (Refer to Library speed comparison in the extras folder for timing details):

> Improvements in speed in v3.0.0 when compared to v2.7.0 (values in percentage of time v3.0.0 is faster than v2.7.0)
>
>| Function | Change in speed |
>| :-----: | :-----: |
>| writeByte | +3% |
>| writeChar | +6% |
>| writeWord | +3% |
>| writeShort | +5% |
>| writeULong | +7% |
>| writeLong | +7% |
>| readLong | +7% |
>|  writeFloat | +7% |
>| writeStr | +57% |
>| readStr | +61% |
>| writeAnything<br>(struct) | +25% |
>| readAnything <br>(struct) | +20% |
>| writeByteArray<br>(256 bytes) | +18% |
>| readByteArray<br>(256 bytes) | +45% |
>| eraseSector | +99% |
>| eraseBlock32K | +99.4% |
>| eraseBlock64K | +99.5% |
>| eraseChip | +12% |

- Constructor changed to enable user to choose one of multiple SPI ports - if available. Look at wiki for further info
- When `RUNDIAGNOSTIC` is uncommented in SPIFlash.h, users now have access to a new function called `flash.functionRunTime()` which can be called after any library I/O function is run. `flash.functionRunTime()` returns the time taken by the previous function to run, in microseconds (as a float). An example use case can be found when the FlashDiagnostics sketch is run with `RUNDIAGNOSTIC` uncommented.
- `_notBusy()` is faster
- Completely re-written `FlashDiagnostics.ino` - uses fewer resources, compatible with more chips and boards
- All functions except the Byte/Char Array Read/Write functions now call an internal `_write`/`_read` function for increased ease of compilation, execution and troubleshooting
- Restructured the internal `_troubleshoot()` function to be better human readable and faster to compile.
- Added a function `getUniqueID()` which returns the unique ID of the flash memory chip as a 64-bit integer.
- Added an internal `_readStat3()` function to enable 4-byte addressing when using flash memory larger than 128 MB
- 4-byte addressing enabled in flash memory larger than 128 MB. This is currently only supported on the W25Q256 and W25Q512. More chips will be supported on a case-by-case basis
- `getAddress()` function now works anytime it is called - even if there is preexisting data on the chip

<hr>

## Version 2.7.0

#### Release date 17.04.2017

###### Bugs Squashed:
- ATTiny85 support available again.

###### New Boards supported:
- Arduino Zero now fully supported.

###### Enhancements:
- Compatible with the ESP32 core for Arduino as of the current commit [9618eec](https://github.com/espressif/arduino-esp32/tree/9618eec19ea470bb59b9a25ebeb8c0c516ca89cc) on 02.08.2017. For some unknown reason, SPI clock speeds higher than board speed/4 are not stable and so, the clock speed for ESP32 dev boards has currently been throttled to 20MHz.

	<small> _NOTE: ESP32 boards usually have an SPIFlash already attached to their SS pin, so the user has to declare the ChipSelect pin being used when the constructor is declared - for example
`SPIFlash flash(33);`_ </small>

<hr>

## Version 2.6.0

#### Release date 16.04.2017

###### Bugs Squashed:
- Fixed issue with reading status register 2 and the Suspend operation where the library was not checking the suspend status correctly.

###### Deletions:
- `flash.getChipName()` has been removed as it is superfluous now that the library is now on its way to multi-chip compatibility.
- Library is not compatible with the ATTiny85 anymore. Currently working on a fix which will hopefully be rolled out in v2.7.0

###### New Boards supported:
- RTL8195A compatibility tested and enabled by @boseji on 02.03.17. Code modified to fit with the library structure by Prajwal Bhattaram on 14.04.17.
- Compatible with the ESP32 core for Arduino as of the current commit [7d0968c](https://github.com/espressif/arduino-esp32/tree/7d0968c10dc540709d69a28eab2457075c1e4fa8) as on 16.04.2017. The ESP32 core currently does not support analogRead and so randomSeed(AnalogRead(A0)) cannot be used.

	<sub> For an unknown reason, SPI clock speeds higher than board speed/4 are not stable and so, the clock speed for ESP32 dev boards has currently been throttled to 20MHz. </sub>
- Added support for the Simblee module.

###### Enhancements:
- `flash.error()` now takes an optional argument 'VERBOSE'. By default the verbosity argument  is set to 'false' and calling `flash.error()` still returns the latest error code as an unsigned byte. However, running `flash.error(VERBOSE)` will not only return the latest error code, but will also print it to serial. This - especially in boards with resources to spare - will result in a detailed error report printed to serial.
- `flash.begin()` now returns a boolean value to indicate establishment of successful comms with the flash chip. Usercode can be made more efficient now by calling ```if (!flash.begin()) { Serial.println(flash.error(VERBOSE)); }``` to identify a problem as soon as the library code is run.
- The internal `_addressCheck()` function now locks up usercode with appropriate error codes if
	1. `flash.begin()` has not been called (or)
	2. There is a possible issue with the wiring - i.e. the flash chip is non-responsive (or)
	3. If the chip's capacity either cannot be identified & the user has not defined a chipSize in `flash.begin()`.
- Fixed `powerDown()`to be more efficient. The chip now powers down much faster than before.
- Added a new error code. Library can now detect non-responsive chips - bad wiring or otherwise.
- Streamlined variables to make code structure better.
- Changed library structure to enable the addition of multi-flash compatibility in the near future.
- moved `#define RUNDIAGNOSTIC` & `#define HIGHSPEED` to `SPIFlash.h` from `SPIFlash.cpp`.
- Now works with other Winbond modules (not in the official supported module list) (beta) by taking the flash memory size in bits as an argument in `flash.begin(_chipSize)`;
- Formatted code to be better human readable.
- Changed the internal `_troubleshoot()` function to use fewer resources.


<hr>

## Version 2.5.0

#### Release date 13.11.2016

###### Bugs Squashed:
- Errorchecking on `writeByteArray()` and `writeCharArray()` was broken in 2.4.0 and is fixed now.
- Previous versions (< 2.5.0) suffered from the inability to write datasets that spanned page boundaries. This has now been fixed (Ref. Enhancements section below)

###### Deletions:
- `flash.writePage()` & `flash.readPage()` have been retired as they can be duplicated - more effectively - by other functions. Refer to `pageDiag()` in `Diagnostics.ino` to see how the same results can be obtained by using `flash.writeByteArray()` & `flash.readByteArray()`.

###### Enhancements:
- Added support for SPI Transactions.
- Added an upgrade for Arduino Due users to read and write large arrays of data at very high speeds - previously reading 60kb of data would take ~108ms - it now takes ~525uS. In this mode, the time between consecutive reads has been reduced to <12ms (needs more work!).
- Completely re-written `writeByteArray()` and `writeCharArray()` functions. They no longer breakdown when writing arrays that span multiple pages.
- All functions that write more than 1 byte of data now take into account page boundaries when writing across pages. In previous versions writing across a page boundary would cause the remainder of the data to wrap around to the beginning of the page. Now the data is written seamlessly from one page over to the next.
- Calling `flash.begin()` now instantiates the SPI bus. Comms with other SPI slaves do not need to call `SPI.begin()` if `flash.begin()` has already been called first in the user's code.
- Sped up `_beginSPI()`. Code in `_beginSPI()` instantiates the SPI bus if another SPI comms library has shut it down.
- Added function allowing setting SPI clock speeds - call the function `flash.setClock(uint32_t clockSpeed)` straight after `begin()`.
- Made `_beginSPI()` & `_nextByte()` shorter and faster.
- Replaced all calls to `SPI.transfer()` with `xfer()` to make the code more human readable and to enable simplicity when switching platforms.
- Added the ability to change Chip Erase wait times depending on the chip (supported chips only).
- Don't need to `#include <SPI.h>` in user code anymore. Just `#include<SPIFlash.h>` will do. Examples are now updated.
- Now supports & uses `SPI.transfer(*buf, count)` along with the standard `SPI.transfer()`, speeding up the read/write of anything larger than an byte (AVR Boards only)
--> Reduces the program storage space in the uC's flash (AVR Boards only)

<hr>

## Version 2.4.0

#### Release date 11.09.2016

###### Bugs Squashed:
- Fixed bug preventing writing to Address 0x00
- Fixed bug in `_notPrevWritten()` which did did not perform a thorough enough check to see if a location had been previously written to.
- Fixed errorchecking bug - it now works for all data types - not just byte/char as previously.

###### Enhancements & Optimizations:
- Added a function 'error()' to enable users to check for errors generated at any point in their program.
- Optimized writePage() so it doesn't depend on other functions and so runs faster than before.
- Diagnostics.ino now outputs time taken for each function as a part of its and provides additional diagnostic data.
- Now have a common set of private functions:
	* `_prep()`
	* `_beginSPI()`
	* `_nextByte()`
	* `_endSPI()`

	to replace

	* `_prepWrite()`
	* `_prepRead()`
	* `_beginRead()`
	* `_readNextByte()`
	* `_beginWrite()`
	* `_writeNextByte()` &
	* `_endProcess()`
- Changed the way `_addressCheck()` works. It now checks all addresses involved in writing the data rather than one address block at a time and returns the correct address to write to in case overflow is enabled. Also, this function is now built into `_prep()`.
- Reading and writing using page numbers + offset has been optimised to be faster than before.
- Optimized the way address and error checking is done so its more efficient and uses few system resources and runs faster.
- Using this library with an ESP8266 board defaults to using GPIO15 as the Slave Select - unless something different is explicitly specified in the constructor.

<hr>

## Version 2.3.1

#### Release date 19.06.2016

- W25Q256FV support added. (Thanks [@Stanislav-Povolotsky](https://github.com/Marzogh/SPIMemory/pull/26)!)
- Cleaned up redundant code in TestFlash.ino and SPIFlash.h

<hr>

## Version 2.3.0

#### Release date 04.06.2016

- ESP8266 compatibility added
- Compatible with Arduino Fio and Micro
- Now compatible with Arduino IDE v1.6.9
- Compatible with Arduino-Makefile. (Thanks [@rnestler](https://github.com/Marzogh/SPIMemory/pull/21) !)

<hr>

## Version 2.2.0

#### Release date 24.11.2015

- When `#RUNDIAGNOSTIC` is uncommented, the `_troubleshoot()` function is now optimised for different µCs
- Added the ability to check if the address has been previously written to before initiating a write
	operation.
- Added a `sizeofStr()` function to get sizes of String objects, to use with the `getAddress()` function
- Fixed a bug with `getAddress()`
- Added the ability get the chip's name via `getChipName()`
- `Diagnostics.ino` has been made more and efficient and provides a cleaner Serial output
- Added `getAddressEx.ino` to show how `getAdress()` works.

<hr>

## Version 2.1.1

#### Release date 24.10.2015

- Fixed bugs that prevented the following functions from working properly

	* `powerDown()`
	* `powerUp()`
	* `suspendProg()`
	* `resumeProg()`

<hr>

## Version 2.1.0

#### Release date 18.10.2015

- Arduino Due compatible - refer to wiki for further details
- Fixed bug with write/readByteArray
- Added write/readCharArray
- Added a proper error checking function that gets called when #ifdef RUNDIAGNOSTIC is uncommented in SPIFlash.cpp.
	This function returns a verbose error message to the Serial console instead of the terse error codes of the previous version.
- The following functions have been changed to enable bug fixes and uniformity in coding style.

	* `writeBytes()`
	* `readBytes()`

	They have been replaced with

	* `readByteArray()`
	* `writeByteArray()`

	respectively

<hr>

## Version 2.0.0

#### Release date 12.10.2015

- Sped up all functions atleast 25x
- Compatible with ATTiny85
- All Read/Write/Erase functions can now take either (page number & offset) or (address) as arguments (Except `readPage()` & `printPage()`)
- `getAddress()` can now return either a 32-bit address or a page number & offset - Refer to Readme.md
- Error codes explained:
		0x00	SUCCESS 		Operation successful.
		0x01	CALLBEGIN		Please make sure `begin()` has been called in `setup()`.
		0x02	UNKNOWNCHIP		Unknown chip manufacturer.
		0x03	UNKNOWNCAP		Unknown chip capacity.
		0x04	CHIPBUSY		Chip busy.
		0x05	OUTOFBOUNDS		Address out of bounds. Please check if `begin()` has been called in `setup()`.
		0x06	CANTENWRITE		Unable to `_writeEnable()`. Check wiring/chip.
		0x07	OUTOFMEM		Pagenumber outside maximum.
		0x08	OUTOFPAGE		Offset is outside page.
- The following functions are deprecated to enable compatibility with other AVR chips.

	* `_printPageBytes()`
	* `printPage()`
	* `printAllPages()`
	* `readSerialStr()`

	<sub> They can be used by uncommenting them in the SPIFlash.cpp file. However, be warned, this particular block of code has only been tested with the Arduino	IDE (1.6.5) and only with 8-bit AVR based Arduino boards and will not be supported any further.</sub>

<hr>

## Version 1.3.2

#### Release date 09.10.2015


- Added the ability to read and write String objects with a simple high level function `readStr()` & `writeStr()`
- Added the ability to `getAddress()`
- Added the ability to fastRead to every read function as the last boolean argument (defaults to FALSE)
- Changed the example code as given below:
	- Modified `TestFlash.ino` to use 25% less memory
	- `Struct_writer.ino` now writes struct to a random location on the Flash Memory chip.
	- Added instructions for real world data storage to `Struct_writer.ino`
	- `Diagnostics.ino` now provides a cleaner diagnostic readout
