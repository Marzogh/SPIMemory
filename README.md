# SPIFlash [![Build Status](https://travis-ci.org/Marzogh/SPIFlash.svg?branch=v1.3.2-w.i.p)](https://travis-ci.org/Marzogh/SPIFlash)
### Arduino library for Winbond Flash Memory Chips
<sup> Download the latest stable release (v1.3.2) from <a href = "https://github.com/Marzogh/SPIFlash/releases/latest">here</a>. Please report any bugs in issues.</sup>

This library is for a the Winbond serial flash memory chips. In its current form it enables reading and writing bytes from and to various locations; reading and writing pages of bytes; continous reading/writing of data from/to arrays of bytes; sector, block and chip erase; and powering down for low power operation.

- For details of the Winbond Flash chips compatible with this library please refer to the Excel spreadsheet in the Extras folder.

#### Installation
- Click on the 'Download zip' button to the right.
- Unzip the archive and rename resulting folder to 'SPIFlash'
- Move the folder to your libraries folder (~/sketches/libraries)

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
Must be called at the start in setup(). This function detects the type of chip being used and sets parameters accordingly
###### getID()
Fetches the JEDEC ID as a 32 bit number.
<hr>
##### Read commands
All read commands take a last boolean argument 'fastRead'. This argument defaults to FALSE, but when set to TRUE carries out the Fast Read instruction so data can be read at up to the memory's maximum frequency.
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
###### readStr(page_number, offset, data)
Reads a _String_ (String Object) from a specific location on a page to the supplied String object. Takes the page number (0-maxPage), the offset of the String within page (0-255) and a String to save the data into, as arguments.
###### readPage(page_number, *data_buffer)
Reads a page worth of data into a data buffer array for further use. ```uint8_t data_buffer[256];``` The data buffer **must** be an array of 256 bytes.
###### ReadAnything(page_number, offset, value)
Reads _any type of variable/struct_ (any sized value) from a specific location on a page. Takes the page number (0-maxPage), the offset of the data within page (0-255) and the variable/struct to save the data into, as arguments.
<hr>
##### Write commands
All write commands take a boolean last argument 'errorCheck'. This argument defaults to TRUE, but when set to FALSE will more than double the writing speed. This however comes at the cost of checking for writing errors. Use with care.
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
Writes _any type of variable/struct_ (any sized value) to a specific location on a page. Takes the page number (0-maxPage), the offset of the data within page (0-255) and the variable/struct to write the data from, as arguments.
<hr>
##### Continuous read/write commands
###### readBytes(page_number, offset, *data_buffer)
Reads an array of bytes starting from a specific location in a page. Takes the page number (0-maxPage), offset of data byte within page (0-255) and a data_buffer - i.e. an array of bytes to be read from the flash memory - as arguments. ```uint8_t data_buffer[n];``` The data buffer **must** be an array of n **bytes**. 'n' is determined by the amount of storage available on the Arduino board.
###### writeBytes(page_number, offset, *data_buffer)
Writes an array of bytes starting from a specific location in a page. Takes the page number (0-maxPage), offset of data byte within page (0-255) and a data_buffer - i.e. an array of bytes to be written to the flash memory - as arguments. ```uint8_t data_buffer[n];``` The data buffer **must** be an array of 'n' **bytes**. The number of bytes - 'n' - is determined by the amount of storage available on the Arduino board.
<hr>
##### Erase commands
###### eraseSector(page_number)
Erases one 4KB sector - 16 pages - containing the page to be erased. The sectors are numbered 0 - 255 containing 16 pages each.
Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255, and so on...
###### eraseBlock32K(page_number)
Erases one 32KB block - 128 pages - containing the page to be erased. The blocks are numbered 0 - 31 containing 128 pages each.
Page 0-127 --> Block 0; Page 128-255 --> Block 1;......Page 3968-4095 --> Block 31, and so on...
###### eraseBlock64K(page_number)
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
Wakes chip from low power state.
<hr>
##### printPage(page_number, outputType)
Reads a page worth of data into a data buffer array for further useand prints to a Serial stream at 115200 baud by default. (The baudrate can be changed by calling ```Serial.begin()``` at a different baudrate in ``` void setup()```)Setting an outputType of 1 enables output in hexadecimal while an outputType of 2 enables output in decimal, CSV - over Serial.
<hr>
##### printAllPages()
Reads all pages on Flash chip and dumps it to Serial stream. This function is useful when extracting data from a flash chip onto a computer as a text file.

###### How to get data off Flash memory via Serial
<sub>(Works only for Unix based Operating Systems)</sub>

 - Make sure you include code to let you dump entire flash memory/specific page's content to Serial (Refer to the code in the _TestFlash.h_ for details on how to do this)
 - Connect your arduino to the computer.
 - Open the Arduino IDE, the IDE's Serial Monitor, and an OSX POSIX terminal.
 - Type the following command into the terminal window. ```% tail -f /dev/tty.usbmodem1411 > FlashDump.txt``` Make sure to replace the _/dev/tty.usbmodem1411_ with the port your arduino is connected to. (You can find this in Tools --> Ports in Arduino IDE 1.6.x)
 - Then type the command to read all pages into the Serial console. If you use my code from the example file the command is ```read_all_pages```
 - Wait a few seconds before typing ```Ctrl+C``` to end the tail process
 - Check that you have actually recieved all the data by typing ```% cat FlashDump.txt```. This should output the entire textfile into your terminal window.



