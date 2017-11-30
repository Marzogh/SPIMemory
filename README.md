# SPIFlash [![Build Status](https://travis-ci.org/Marzogh/SPIFlash.svg?branch=stable)](https://travis-ci.org/Marzogh/SPIFlash) [![DOI](https://zenodo.org/badge/18908/Marzogh/SPIFlash.svg)](https://zenodo.org/badge/latestdoi/18908/Marzogh/SPIFlash)
[![GitHub release](https://img.shields.io/github/release/Marzogh/SPIFlash.svg)](https://github.com/Marzogh/SPIFlash)
[![GitHub commits](https://img.shields.io/github/commits-since/Marzogh/SPIFlash/v3.0.0.svg)](https://github.com/Marzogh/SPIFlash/compare/v3.0.0...development)
[![GitHub issues](https://img.shields.io/github/issues/Marzogh/SPIFlash.svg)](https://github.com/Marzogh/SPIFlash/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/Marzogh/SPIFlash.svg)](https://github.com/Marzogh/SPIFlash/pulls)
[![license](https://img.shields.io/github/license/Marzogh/SPIFlash.svg)](https://github.com/Marzogh/SPIFlash/blob/master/LICENSE)

### Arduino library for Flash Memory Chips (SPI based only)
<sup> Download the latest stable release (v3.0.0) from <a href = "https://github.com/Marzogh/SPIFlash/releases/latest">here</a>. Please report any bugs in issues.</sup>

This Arduino library is for use with flash memory chips that communicate using the SPI protocol. In its current form it supports identifying the flash chip and its various features; automatic address allocation and management; writing and reading a number of different types of data, ranging from 8-bit to 32-bit (signed and unsigned) values, floats, Strings, arrays of bytes/chars and structs to and from various locations; sector, block and chip erase; and powering down for low power operation.

- For details of the Flash chips compatible with this library please refer to the list below.

#### Compatibility

##### Arduino IDEs supported (actually tested with)
- IDE v1.5.x
- IDE v1.6.0-v1.6.5
- IDE v1.6.9-v1.6.12
- IDE v1.8.1-v1.8.5

##### Boards

###### Completely supported
- Arduino Uno
- Arduino Leonardo
- Arduino Due
- Arduino Zero
- Nucleo-F091RC
- Adafruit Feather M0
- Adafruit Feather M0 Express
- ESP8266 Boards (On the Arduino IDE)
- Simblee Boards (On the Arduino IDE)
- Arduino Mega
- Arduino Micro
- Arduino Fio

###### In BETA
- ESP32 Boards (Tested on the Adafruit ESP32 Feather) The library is known to work with the ESP32 core as of the current commit <a href = "https://github.com/espressif/arduino-esp32/commit/8ba91b945330303f9013f30da75c49c4d6dd8a09">8ba91b9</a> on 07.11.2017. ```ESP32 support will remain in beta till the ESP32 core can be installed via the Arduino boards manager.```
NOTE: ESP32 boards usually have an SPIFlash already attached to their SS pin, so the user has to declare the ChipSelect pin being used when the constructor is declared - for example
```
SPIFlash flash(33);
```

##### Flash memory compatibility

###### Completely supported (Actually tested with)
- Winbond
  - W25Q16BV
  - W25Q64FV
  - W25Q80BV
  - W25Q256FV
- Microchip
  - SST25VF064C
  - SST26VF064B
- Cypress/Spansion
  - S25FL116K
  - S25FL127S

###### Should work with (Similar enough to the ones actually tested with)
- Winbond (All SPI Flash chips)
- Microchip (SST25 & SST26 series)
- Cypress/Spansion (S25FL series)

#### Installation

##### Option 1
- Open the Arduino IDE.
- Go to Sketch > Include Library > Manage libraries.
- Search for SPIFlash.
- Install the latest version.

##### Option 2
- Click on the 'Clone or download' button above the list of files on this <a href = "https://github.com/Marzogh/SPIFlash/tree/master"> page </a>.
- Select Download ZIP. A .zip file will download to your computer.
- Unzip the archive and rename resulting folder to 'SPIFlash'
- Move the folder to your libraries folder (~/sketches/libraries)

#### Usage

- The library is called by declaring the```SPIFlash flash(csPin*)``` constructor where 'flash' can be replaced by a user constructor of choice and 'csPin' is the Chip Select pin for the flash module.

    <sub>* Optional. Do not include csPin if using the default slave select pin for your board.</sub>
- Every version of the library >= v3.0.0 supports the ability to use any of multiple SPI interfaces (if your micro-controller supports them). Switching to use another SPI interface is done by calling ```SPIFlash flash(csPin, &SPI1);``` (or &SPI2 and so on), instead of ```SPIFlash flash(csPin)```.

    <sub>* NOTE: This is currently only supported on the SAMD and STM32 architectures.</sub>
- Make sure to include ```#include<SPI.H>``` when you include ```#include<SPIFlash.h>```.
- Also make sure to include ```flash.begin(CHIPSIZE*)``` in ```void setup()```. This enables the library to detect the type of flash chip installed and load the right parameters.

    <sub>* Optional</sub>

###### Notes on Address overflow and Error checking
- The library has Address overflow enabled by default - i.e. if the last address read/written from/to,  in any function, is 0xFFFFF then, the next address read/written from/to is 0x00000. This can be disabled by uncommenting ```#define DISABLEOVERFLOW``` in SPIFlash.h. (Address overflow only works for Read / Write functions. Erase functions erase only a set number of blocks/sectors irrespective of overflow.)
- All write functions have Error checking turned on by default - i.e. every byte written to the flash memory will be checked against the data stored on the Arduino. Users who require greater write speeds can disable this function by setting an optional last 'errorCheck' argument in any write function to NOERRCHK - For eg. call the function ```writeByte(address, *data_buffer, NOERRCHK)``` instead of ```writeByte(address, *data_buffer)```.

The library enables the following functions:
<hr>

##### Non-I/O commands
<hr>

###### begin(_chipsize*)
Must be called at the start in setup(). This function detects the type of chip being used and sets parameters accordingly. An optional CHIPSIZE parameter can be declared as an argument with this function. For supported CHIPSIZE values, please refer to the appropriate [wiki section](https://github.com/Marzogh/SPIFlash/wiki/begin()) or look at defines.h .

###### setClock(clockSpeed)
Must be called straight after begin(). This function takes a 32-bit number as replacement for the default maximum clock speed (104MHz for Winbond NOR flash) thereby initiating future SPI transactions with the user-defined clock speed. Use with care.

###### error(_verbosity)
Returns the (8-bit) error code generated by the function called immediately before this is called. If the optional VERBOSE argument is used, a verbose troubleshooting report is printed to Serial. Refer to the [Error reporting](https://github.com/Marzogh/SPIFlash/wiki/Error-reporting) section the Wiki for further reference.

###### getMANID()
Returns the Manufacturer ID as a 16-bit value.

###### getJEDECID()
Returns the JEDEC ID as a 32-bit value.

###### getUniqueID()
Returns the Unique ID as a 64-bit value.

###### getAddress(sizeOfData)
Gets the next available address for use. Has two variants:
* Takes the size of the data as an argument and returns a 32-bit address
* Takes a three variables, the size of the data and two other variables to return a page number value & an offset into.

In library versions >= v3.0.0 This function can be called anytime - even if there is preexisting data on the flash chip. It will simply find the next empty address block for the data.

###### sizeofStr()
Use this function to find the size of a String to use as the argument in getAddress(size). Using size = sizeof(String) will cause the getAddress(size) function to fail.

size = sizeof(variable) can be used for all types of data but String objects.

###### getCapacity()
Returns the capacity in bytes as a 32-bit value.

###### getmaxPage()
Returns the maximum number of pages in the flash memory as a 32-bit value.

###### functionRunTime()
Returns the time taken to run a function. Must be called immediately after a function is run as the variable returned is overwritten each time a function from this library is called. Primarily used in the diagnostics sketch included in the library to track function time.
<sub> NOTE: Can only be called when '#define RUNDIAGNOSTIC' is uncommented in SPIFlash.h</sub>

<hr>

##### Read commands
<hr>
<sub>
All read commands take a last boolean argument 'fastRead'. This argument defaults to FALSE, but when set to TRUE carries out the Fast Read instruction so data can be read at up to the memory's maximum frequency.
</sub>

<sub>
All read commands only take a 32-bit address variable instead of the optional 16-bit page number & 8-bit offset variables in previous versions of the library (< v3.0.0)
</sub>

###### readAnything(address, value)
Reads _any type of variable/struct_ (any sized value) from a specific location. Takes the address (0-maxAddress) of the data and the variable/struct to write the data back to, as arguments.

###### readByte(address)
Reads a _byte_ (unsigned 8 bit value) from a specific location. Takes the address (0-maxAddress) of the byte as an argument.

###### readChar(address)
Reads a _char_ (signed 8 bit value) from a specific location. Takes the address (0-maxAddress) of the char as an argument.

###### readWord(address)
Reads a _word_ (unsigned 16 bit value) from a specific location. Takes the address (0-maxAddress) of the word as an argument.

###### readShort(address)
Reads a _short_ (signed 16 bit value) from a specific location. Takes the address (0-maxAddress) of the short as an argument.

###### readULong(address)
Reads an _unsigned long_ (unsigned 32 bit value) from a specific location. Takes the address (0-maxAddress) of the unsigned long as an argument.

###### readLong(address)
Reads a _long_ (signed 32 bit value) from a specific location. Takes the address (0-maxAddress) of the long as an argument.

###### readFloat(address)
Reads a _float_ (decimal value) from a specific location. Takes the address (0-maxAddress) of the float as an argument.

###### readStr(address, outputStr)
Reads a _string_ (String Object) from a specific location to an outputStr variable. Takes the address (0-maxAddress) of the String and a String as arguments.

###### readAnything(address, value)
Reads _any type of variable/struct_ (any sized value) from a specific location. Takes the address (0-maxAddress) of the data and the variable/struct to write the data to, as arguments.
<hr>

##### Write commands
<hr>
<sub>
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.
</sub>

<sub> All write commands only take a 32-bit address variable instead of the optional 16-bit page number & 8-bit offset variables in previous versions of the library (< v3.0.0) </sub>

###### writeByte(address, data)
Writes a byte of data to a specific location. Takes the address (0-maxAddress) of data byte and one byte of data as arguments.

###### writeChar(address, data)
Writes a _char_ (signed 8 bit value) to a specific location. Takes the address (0-maxAddress) of the char and one char of data  as arguments.

###### writeWord(address, data)
Writes a _word_ (unsigned 16 bit value) to a specific location. Takes the address (0-maxAddress) of the word and one word of data as arguments.

###### writeShort(address, data)
Writes a _short_ (signed 16 bit value) to a specific location. Takes the address (0-maxAddress) of the short and one short of data as arguments.

###### writeULong(address, data)
Writes an _unsigned long_ (unsigned 32 bit value) to a specific location. Takes the address (0-maxAddress) of the unsigned long and one unsigned long of data as arguments.

###### writeLong(address, data)
Writes a _long_ (signed 32 bit value) to a specific location. Takes the address (0-maxAddress) of the long and one long of data as arguments.

###### writeFloat(address, data)
Writes a _float_ (decimal value) to a specific location. Takes the address (0-maxAddress) of the float and one float of data as arguments.

###### writeStr(address, data)
Writes a _string_ (String Object) to a specific location. Takes the address (0-maxAddress) of the String and a String as arguments.

###### writeAnything(address, value)
Writes _any type of variable/struct_ (any sized value) from a specific location. Takes the address (0-maxAddress) of the data and the variable/struct to write the data from, as arguments.
<hr>

##### Continuous read/write commands
<sub>
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.
</sub>
<hr>

###### readByteArray(address, *data_buffer, bufferSize)
Reads an array of bytes starting from a specific address Takes the address (0-maxAddress) & a data_buffer - i.e. an array of bytes to be read from the flash memory - and size of the array as arguments. ```uint8_t data_buffer[n];``` The data buffer **must** be an array of n **bytes**. 'n' is determined by the amount of RAM available on the Arduino board.

###### writeByteArray(address, *data_buffer, bufferSize)
Writes an array of bytes starting from a specific address. Takes the address (0-maxAddress) & a data_buffer - i.e. an array of bytes to be written to the flash memory - and size of the array as arguments. ```uint8_t data_buffer[n];``` The data buffer **must** be an array of 'n' **bytes**. The number of bytes - 'n' - is determined by the amount of RAM available on the Arduino board.

###### readCharArray(address, *data_buffer, bufferSize)
Reads an array of chars starting from a specific address. Takes the address (0-maxAddress) & a data_buffer - i.e. an array of chars to be read from the flash memory - and size of the array as arguments. ```char data_buffer[n];``` The data buffer **must** be an array of n **chars**. 'n' is determined by the amount of RAM available on the Arduino board.

###### writeCharArray(address, *data_buffer, bufferSize)
Writes an array of chars starting from a specific address. Takes the address (0-maxAddress) & a data_buffer - i.e. an array of chars to be written to the flash memory - and size of the array as arguments. ```char data_buffer[n];``` The data buffer **must** be an array of 'n' **chars**. The number of chars - 'n' - is determined by the amount of RAM available on the Arduino board.
<hr>

##### Erase commands
<sub>
All erase commands only take a 32-bit address variable instead of the optional 16-bit page number & 8-bit offset variables in previous versions of the library (< v3.0.0)
</sub>
<hr>

###### eraseSector(address)
Erases one 4KB sector - 16 pages - containing the address to be erased. The sectors are numbered 0 - 255 containing 16 pages each.
Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255, and so on...

###### eraseBlock32K(address)
Erases one 32KB block - 128 pages - containing the address to be erased. The blocks are numbered 0 - 31 containing 128 pages each.
Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31, and so on...

###### eraseBlock64K(address)
Erases one 64KB block - 256 pages - containing the address to be erased. The blocks are numbered 0 - 15 containing 256 pages each.
// Page 0-255 --> Block 0; Page 256-511 --> Block 1;......Page 3840-4095 --> Block 15, and so on...

###### eraseChip()
Erases entire chip. Use with care.
<hr>

##### Suspend/Resume commands
<hr>

###### suspendProg()
Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase(). Page Program, Write Status Register, Erase instructions are not allowed. Erase suspend is only allowed during Block/Sector erase. Program suspend is only allowed during Page/Quad Page Program

###### resumeProg()
Resumes previously suspended Block Erase/Sector Erase/Page Program.
<hr>

##### Power operation commands
<hr>

###### powerDown()
Puts device in low power state. Useful for battery powered operations. Typical current consumption during power-down is 1mA with a maximum of 5mA. (Datasheet 7.4). In powerDown() the chip will only respond to powerUp()

###### powerUp()
Wakes chip from low power state.
<hr>

##### Error codes explained

* **0x00**    -    Action completed successfully. No Error.
* **0x01**    -    *constructor_of_choice*.begin() was not called in void setup()
* **0x02**    -    Unable to identify chip. Are you sure this is a Winbond Flash chip? Please raise an issue [here](http://www.github.com/Marzogh/SPIFlash/issues) with your chip type and I will try to add support to your chip
* **0x03**    -    Unable to identify capacity. Please raise an issue [here](http://www.github.com/Marzogh/SPIFlash/issues) with your chip type and I will work on adding support to your chip
* **0x04**    -    Chip is busy. Make sure all pins have been connected properly. If it still doesn't work,please raise an issue [here](http://www.github.com/Marzogh/SPIFlash/issues) with the details of what your were doing when this error occurred
* **0x05**    -    Page overflow has been disabled and the address called exceeds the memory
* **0x06**    -    Unable to Enable Writing to chip. Please make sure the HOLD & WRITEPROTECT pins are connected properly to VCC & GND respectively. If you are still facing issues, please raise an issue [here](http://www.github.com/Marzogh/SPIFlash/issues) with the details of what your were doing when this error occurred
* **0x07**    -    This sector already contains data. Please make sure the sectors being written to are erased. If you are still facing issues, please raise an issue [here](http://www.github.com/Marzogh/SPIFlash/issues) with the details of what your were doing when this error occurred.
* **0x08**    -    You are running low on SRAM. Please optimise your program for better RAM usage
* **0x09**    -    Unable to suspend/resume operation.
* **0x0A**    -    Write Function has failed errorcheck.
* **0x0B**    -    Check your wiring. Flash chip is non-responsive.
* **0x0C**    -    This function is not supported by the flash memory hardware.
* **0x0D**    -    Unable to enable 4-byte addressing.
* **0x0E**    -    Unable to disable 4-byte addressing.
* **0xFE**    -    Unknown error. Please raise an issue [here](http://www.github.com/Marzogh/SPIFlash/issues) with the details of what your were doing when this error occurred
<hr>

###### How to get data off Flash memory via Serial
<sub>(Works only for Unix based Operating Systems)</sub>

 - Make sure you include code to let you dump entire flash memory/specific address' content to Serial (Refer to the code in the _TestFlash.h_ for details on how to do this)
 - Connect your Arduino board to the computer.
 - Open the Arduino IDE, the IDE's Serial Monitor, and an OSX POSIX terminal.
 - Type the following command into the terminal window. ```% tail -f /dev/tty.usbmodem1411 > FlashDump.txt``` Make sure to replace the _/dev/tty.usbmodem1411_ with the port your arduino is connected to. (You can find this in Tools --> Ports in Arduino IDE 1.6.x)
 - Then type the command to read all pages into the Serial console. If you use my code from the example file the command is ```read_all_pages```
 - Wait a few seconds before typing ```Ctrl+C``` to end the tail process
 - Check that you have actually received all the data by typing ```% cat FlashDump.txt```. This should output the entire text file into your terminal window.
