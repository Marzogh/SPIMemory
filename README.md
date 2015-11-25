
# SPIFlash [![Build Status](https://travis-ci.org/Marzogh/SPIFlash.svg?branch=master)](https://travis-ci.org/Marzogh/SPIFlash) [![DOI](https://zenodo.org/badge/18908/Marzogh/SPIFlash.svg)](https://zenodo.org/badge/latestdoi/18908/Marzogh/SPIFlash)
### Arduino library for Winbond Flash Memory Chips
<sup> Download the latest stable release (v2.2.0) from <a href = "https://github.com/Marzogh/SPIFlash/releases/latest">here</a>. Please report any bugs in issues.</sup>

This Arduino library is for use with Winbond serial flash memory chips. In its current form it supports identifying the flash chip and its various features; automatic address allocation and management; reading and writing bytes/chars/ints/longs/floats/Strings from and to various locations; reading and writing pages of bytes; continous reading/writing of data from/to arrays of bytes/chars; sector, block and chip erase; and powering down for low power operation.

- For details of the Winbond Flash chips compatible with this library please refer to the Excel spreadsheet in the Extras folder.

####Compatibility
#####Arduino IDEs supported
- IDE v1.5.x
- IDE v1.6.0-v1.6.5

#####Boards

######Completely supported
- Arduino Uno
- Arduino Leonardo
- Arduino Mega
- Arduino Due
- ATTiny85

####Installation

#####Option 1
- Click on the 'Download zip' button to the right.
- Unzip the archive and rename resulting folder to 'SPIFlash'
- Move the folder to your libraries folder (~/sketches/libraries)

#####Option 2
- Open the Arduino IDE.
- Go to Sketch > Include Library > Manage libraries.
- Search for SPIFlash.
- Install the latest version.

#### Usage

The library is called by declaring the```SPIFLASH flash(csPin)``` constructor where 'flash' can be replaced by a user constructor of choice and 'csPin' is the Chip Select pin for the flash module.
Make sure to include ```#include<SPI.H>``` when you include ```#include<SPIFlash.h>```.
Also make sure to include ```flash.begin()``` in ```void setup()```. This enables the library to detect the type of flash chip installed and load the right parameters.

###### Notes on Address overflow and Error checking
- The library has Address overflow enabled by default - i.e. if the last address read/written from/to,  in any function, is 0xFFFFF then, the next address read/written from/to is 0x00000. This can be disabled by setting the optional last 'overflow' argument in the constructor to false - For eg. call the constructor  ```SPIFlash(csPin, false)```  instead of ```SPIFlash(csPin)```.
- All write functions have Error checking turned on by default - i.e. every byte written to the flash memory will be checked against the data stored on the Arduino. Users who require greater write speeds can disable this function by setting an optional last 'errorCheck' argument in any write function to false - For eg. call the function ```writePage(page_number, *data_buffer, false)``` instead of ```writePage(page_number, *data_buffer)```.

The library enables the following functions:
<hr>
##### Primary commands
###### begin()
Must be called at the start in setup(). This function detects the type of chip being used and sets parameters accordingly.
###### getMANID()
Returns the Manufacturer ID as a 16-bit value.
###### getJEDECID()
Returns the JEDEC ID as a 32-bit value.
###### getCapacity()
Returns the capacity in bytes as a 32-bit value.
###### getmaxPage()
Returns the maximum number of pages in the flash memory as a 32-bit value.
###### getChipName()
Returns the identifiying number of the chip's name from which the chip's name can be generated. (Refer to the getID() function in examples/Diagnostics/Diagnostics.ino)
###### getAddress()
Gets the next available address for use. Has two variants:
* Takes the size of the data as an argument and returns a 32-bit address
* Takes a three variables, the size of the data and two other variables to return a page number value & an offset into.

All addresses in the in the sketch must be obtained via this function or not at all.
###### sizeofStr()
Use this function to find the size of a String to use as the argument in getAddress(size). Using size = sizeof(String) will cause the getAddress(size) function to fail.

size = sizeof(variable) can be used for all types of data but String objects.
<hr>
###### All read/write commands can take a 32-bit address variable in the place of the 16-bit page number & 8-bit offset variables.
<hr>
##### Read commands
All read commands take a last boolean argument 'fastRead'. This argument defaults to FALSE, but when set to TRUE carries out the Fast Read instruction so data can be read at up to the memory's maximum frequency.

All read commands can take a 32-bit address variable instead of the 16-bit page number & 8-bit offset variables
###### readAnything(page_number, offset, value)
Reads _any type of variable/struct_ (any sized value) from a specific location on a page. Takes the page number (0-maxPage), the offset of the data within page (0-255) and the variable/struct to write the data back to, as arguments.
###### readByte(page_number, offset)
Reads a _byte_ (unsigned 8 bit value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the byte within page (0-255) as arguments.
###### readChar(page_number, offset)
Reads a _char_ (signed 8 bit value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the char within page (0-255) as arguments.
###### readWord(page_number, offset)
Reads a _word_ (unsigned 16 bit value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the word within page (0-255) as arguments.
###### readShort(page_number, offset)
Reads a _short_ (signed 16 bit value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the short within page (0-255) as arguments.
###### readULong(page_number, offset)
Reads an _unsigned long_ (unsigned 32 bit value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the unsigned long within page (0-255) as arguments.
###### readLong(page_number, offset)
Reads a _long_ (signed 32 bit value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the long within page (0-255) as arguments.
###### readFloat(page_number, offset)
Reads a _float_ (decimal value) from a specific location on a page. Takes the page number (0-maxPage) and offset of the float within page (0-255) as arguments.
###### readStr(page_number, offset, outputStr)
Reads a _string_ (String Object) to a specific location on a page to an outputStr variable. Takes the page number (0-maxPage), the offset of the String within page (0-255) and a String as arguments.
###### readPage(page_number, *data_buffer)
Reads a page worth of data into a data buffer array for further use. ```uint8_t data_buffer[256];``` The data buffer **must** be an array of 256 bytes.
###### readAnything(page_number, offset, value)
Reads _any type of variable/struct_ (any sized value) from a specific location on a page. Takes the page number (0-maxPage), the offset of the data within page (0-255) and the variable/struct to write the data to, as arguments.
<hr>
##### Write commands
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.

All write commands can take a 32-bit address variable instead of the 16-bit page number & 8-bit offset variables
###### writeByte(page, offset, data)
Writes a byte of data to a specific location on a page. Takes the page number (0-maxPage), offset of data byte within page (0-255) and one byte of data as arguments.
###### writeChar(page_number, offset, data)
Writes a _char_ (signed 8 bit value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the char within page (0-255) and one char of data  as arguments.
###### writeWord(page_number, offset, data)
Writes a _word_ (unsigned 16 bit value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the word within page (0-255) and one word of data as arguments.
###### writeShort(page_number, offset, data)
Writes a _short_ (signed 16 bit value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the short within page (0-255) and one short of data as arguments.
###### writeULong(page_number, offset, data)
Writes an _unsigned long_ (unsigned 32 bit value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the unsigned long within page (0-255) and one unsigned long of data as arguments.
###### writeLong(page_number, offset, data)
Writes a _long_ (signed 32 bit value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the long within page (0-255) and one long of data as arguments.
###### writeFloat(page_number, offset, data)
Writes a _float_ (decimal value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the float within page (0-255) and one float of data as arguments.
###### writeStr(page_number, offset, data)
Writes a _string_ (String Object) to a specific location on a page. Takes the page number (0-maxPage), the offset of the String within page (0-255) and a String as arguments.
###### writePage(page_number, *data_buffer)
Writes a page worth of data into a data buffer array for further use. ```uint8_t data_buffer[256];``` The data buffer **must** be an array of 256 bytes.
###### writeAnything(page_number, offset, value)
Writes _any type of variable/struct_ (any sized value) from a specific location on a page. Takes the page number (0-maxPage), the offset of the data within page (0-255) and the variable/struct to write the data from, as arguments.
<hr>
##### Continuous read/write commands
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.

###### readByteArray(page_number, offset, *data_buffer, bufferSize)
Reads an array of bytes starting from a specific location in a page. Takes the page number (0-maxPage), offset of data byte within page (0-255), a data_buffer - i.e. an array of bytes to be read from the flash memory - and size of the array as arguments. ```uint8_t data_buffer[n];``` The data buffer **must** be an array of n **bytes**. 'n' is determined by the amount of RAM available on the Arduino board.
###### writeByteArray(page_number, offset, *data_buffer, bufferSize)
Writes an array of bytes starting from a specific location in a page. Takes the page number (0-maxPage), offset of data byte within page (0-255), a data_buffer - i.e. an array of bytes to be written to the flash memory - and size of the array as arguments. ```uint8_t data_buffer[n];``` The data buffer **must** be an array of 'n' **bytes**. The number of bytes - 'n' - is determined by the amount of RAM available on the Arduino board.
###### readCharArray(page_number, offset, *data_buffer, bufferSize)
Reads an array of chars starting from a specific location in a page. Takes the page number (0-maxPage), offset of data byte within page (0-255), a data_buffer - i.e. an array of chars to be read from the flash memory - and size of the array as arguments. ```char data_buffer[n];``` The data buffer **must** be an array of n **chars**. 'n' is determined by the amount of RAM available on the Arduino board.
###### writeCharArray(page_number, offset, *data_buffer, bufferSize)
Writes an array of chars starting from a specific location in a page. Takes the page number (0-maxPage), offset of data byte within page (0-255), a data_buffer - i.e. an array of chars to be written to the flash memory - and size of the array as arguments. ```char data_buffer[n];``` The data buffer **must** be an array of 'n' **chars**. The number of chars - 'n' - is determined by the amount of RAM available on the Arduino board.
<hr>
##### Erase commands
All erase commands can take a 32-bit address variable instead of the 16-bit page number & 8-bit offset variables
###### eraseSector(page_number, offset)
Erases one 4KB sector - 16 pages - containing the page to be erased. The sectors are numbered 0 - 255 containing 16 pages each.
Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255, and so on...
###### eraseBlock32K(page_number, offset)
Erases one 32KB block - 128 pages - containing the page to be erased. The blocks are numbered 0 - 31 containing 128 pages each.
Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31, and so on...
###### eraseBlock64K(page_number, offset)
Erases one 64KB block - 256 pages - containing the page to be erased. The blocks are numbered 0 - 15 containing 256 pages each.
// Page 0-255 --> Block 0; Page 256-511 --> Block 1;......Page 3840-4095 --> Block 15, and so on...
###### eraseChip()
Erases entire chip. Use with care.
<hr>
##### Suspend/Resume commands
###### suspendProg()
Suspends current Block Erase/Sector Erase/Page Program. Does not suspend chipErase(). Page Program, Write Status Register, Erase instructions are not allowed. Erase suspend is only allowed during Block/Sector erase. Program suspend is only allowed during Page/Quad Page Program
###### resumeProg()
Resumes previously suspended Block Erase/Sector Erase/Page Program.
<hr>
##### Power operation commands
###### powerDown()
Puts device in low power state. Useful for battery powered operations. Typical current consumption during power-down is 1mA with a maximum of 5mA. (Datasheet 7.4). In powerDown() the chip will only respond to powerUp()
###### powerUp()
//Wakes chip from low power state.
<hr>
##### Error codes explained
	- 0x00	SUCCESS 		Operation successful.
	- 0x01	CALLBEGIN		Please make sure .begin() has been called in setup().
	- 0x02	UNKNOWNCHIP	Unknown chip manufacturer.
	- 0x03	UNKNOWNCAP		Unknown chip capacity.
	- 0x04	CHIPBUSY		Chip busy.
	- 0x05	OUTOFBOUNDS	Address out of bounds. Please check if .begin() has been called in setup().
	- 0x06	CANTENWRITE		Unable to _writeEnable. Check wiring/chip.
	- 0x07	OUTOFMEM		Pagenumber outside maximum.
	- 0x08	OUTOFPAGE		Offset is outside page.
<hr>
##### Deprecated functions

The following functions are deprecated to enable compatibility with other AVR chips.
`printPage();`
`printAllPages()`
`readSerialStr()`

They can be used by uncommenting them in the SPIFlash.cpp file. However, be warned, this particular block of code has only been tested with the Arduino	IDE (1.6.5) and only with 8-bit ATMega based Arduino boards and will not be supported any further.

###### printPage(page_number, outputType)
Reads a page worth of data into a data buffer array for further useand prints to a Serial stream at 115200 baud by default. (The baudrate can be changed by calling ```Serial.begin()``` at a different baudrate in ``` void setup()```)Setting an outputType of 1 enables output in hexadecimal while an outputType of 2 enables output in decimal, CSV - over Serial.

###### printAllPages()
Reads all pages on Flash chip and dumps it to Serial stream. This function is useful when extracting data from a flash chip onto a computer as a text file.

###### How to get data off Flash memory via Serial
<sub>(Works only for Unix based Operating Systems)</sub>

 - Make sure you include code to let you dump entire flash memory/specific page's content to Serial (Refer to the code in the _TestFlash.h_ for details on how to do this)
 - Connect your Arduino board to the computer.
 - Open the Arduino IDE, the IDE's Serial Monitor, and an OSX POSIX terminal.
 - Type the following command into the terminal window. ```% tail -f /dev/tty.usbmodem1411 > FlashDump.txt``` Make sure to replace the _/dev/tty.usbmodem1411_ with the port your arduino is connected to. (You can find this in Tools --> Ports in Arduino IDE 1.6.x)
 - Then type the command to read all pages into the Serial console. If you use my code from the example file the command is ```read_all_pages```
 - Wait a few seconds before typing ```Ctrl+C``` to end the tail process
 - Check that you have actually recieved all the data by typing ```% cat FlashDump.txt```. This should output the entire textfile into your terminal window.

[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/Marzogh/spiflash/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

