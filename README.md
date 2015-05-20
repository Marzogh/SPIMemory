# SPIFlash
### Arduino library for Winbond Flash W25Q80BV
<sup> Download the latest stable release (v1.0) from <a href = "https://github.com/Marzogh/SPIFlash/releases/tag/v1.0">here</a>. Please report any bugs in issues.</sup>

This library is for the W25Q80BV serial flash memory chip. In its current form it enables reading and writing bytes from and to various locations, reading and writing pages, sector, block and chip erase, powering down for low power operation and continuous read functions.

- The W25Q80BV has a capacity of 64 Mbits (1 Megabyte) which consists of 4096 pages of 256 bytes each.
- It has 16 blocks of 64KB each
- Byte addresses are 24 bit range from 0x00000 to 0xFFFFF (00000 to 1048575 - decimal)

#### Installation
- Click on the 'Download zip' button to the right.
- Unzip the archive and rename resulting folder to 'SPIFlash'
- Move the folder to your libraries folder (~/sketches/libraries)

#### Usage

The library is called by declaring the```SPIFLASH flash(csPin)``` constructor where 'flash' can be replaced by a user constructor of choice and 'csPin' is the Chip Select pin for the flash module.
Make sure to include ```#include<SPI.H>``` when you include ```#include<SPIFlash.h>```.

The library enables the following functions:
<hr>
##### getID ()
Fetches the JEDEC ID as a 32 bit number.
<hr>
##### Continuous read commands
###### beginRead(address)
Begins the read process but does not actually do any reading. This function **must** be called before readNextByte().
###### readNextByte()
When called repeatedly, this function reads consecutive bytes in the flash chip's memory. This function **must** be called only after beginRead(address)
###### endRead()
Has to be called at the end of the readNextByte() function to end the read process.
<hr>
##### Read commands
###### readByte(page, offset)
Reads a byte of data from a specific location on a page. Takes the page number (0-4095) and offset of data byte within page (0-255) as arguments.
###### readPage(page_number, *page_buffer, outputType)
Reads a page worth of data into a page buffer array for further use. ```uint8_t page_buffer[256];``` The page buffer **must** be an array of 256 bytes. Setting an outputType of 1 enables output in hexadecimal while an outputType of 2 enables output in decimal, CSV - over Serial.
<hr>
##### Write commands
###### writeByte(page, offset, data)
Writes a byte of data to a specific location on a page. Takes the page number (0-4095), offset of data byte within page (0-255) and one byte of data (0-255) as arguments.
###### writePage(uint16_t page_number, uint8_t *page_buffer)
Writes a page worth of data into a page buffer array for further use. ```uint8_t page_buffer[256];``` The page buffer **must** be an array of 256 bytes.
<hr>
##### Erase commands
###### eraseSector(address)
Erases one sector - 16 pages/4KB. The chip rounds this down to the 4KB sector boundary prior to the address - if the address marks the beginning of a 4KB sector, erase will begin there. Byte addresses range from 0x00000 to 0xFFFFF (00000 to 1048575).
###### eraseBlock32K(address)
Erases one 32KB block - 128 pages. The chip rounds this down to the 32KB block boundary prior to the address - if the address marks the beginning of a 32KB sector, erase will begin there.  Byte addresses range from 0x00000 to 0xFFFFF (00000 to 1048575).
###### eraseBlock64K(address)
Erases one 64KB block - 256 pages. The chip rounds this down to the 64KB block boundary prior to the address - if the address marks the beginning of a 64KB sector, erase will begin there.  Byte addresses range from 0x00000 to 0xFFFFF (00000 to 1048575).
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
##### readAllPages()
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



