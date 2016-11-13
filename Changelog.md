//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//										  SPIFlash Library 				 						   //
//					  				  		Changes log 									   	   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.5.0 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						13.11.2016											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Bugs Squashed:
--> Errorchecking on writeByteArray() and writeCharArray() was broken in 2.4.0 and is fixed now.
--> Previous versions (< 2.5.0) suffered from the inability to write datasets that spanned page boundaries. This has now been fixed (Ref. Enhancements section below)

Deletions:
--> flash.writePage() & flash.readPage() have been retired as they can be duplicated - more effectively - by other functions. Refer to pageDiag() in Diagnostics.ino to see how the same results can be obtained by using flash.writeByteArray() & flash.readByteArray().

Enhancements:
--> Added support for SPI Transactions.
--> Added an upgrade for Arduino Due users to read and write large arrays of data at very high speeds - previously reading 60kb of data would take ~108ms - it now takes ~525uS. In this mode, the time between consecutive reads has been reduced to <12ms (needs more work!).
--> Completely re-written writeByteArray() and writeCharArray() functions. They no longer breakdown when writing arrays that span multiple pages.
--> All functions that write more than 1 byte of data now take into account page boundaries when writing across pages. In previous versions writing across a page boundary would cause the remainder of the data to wrap around to the beginning of the page. Now the data is written seamlessly from one page over to the next.
--> Calling flash.begin() now instantiates the SPI bus. Comms with other SPI slaves do not need to call SPI.begin() if flash.begin() has already been called first in the user's code.
--> Sped up _beginSPI(). Code in _beginSPI() instantiates the SPI bus if another SPI comms library has shut it down.
--> Added function allowing setting SPI clock speeds - call the function ```flash.setClock(uint32_t clockSpeed);``` straight after ```begin();```.
--> Made _beginSPI() & _nextByte() shorter and faster.
--> Replaced all calls to SPI.transfer() with xfer() to make the code more human readable and to enable simplicity when switching platforms.
--> Added the ability to change Chip Erase wait times depending on the chip (supported chips only).
--> Don't need to ```#include <SPI.h>``` in user code anymore. Just ```#include<SPIFlash.h>``` will do. Examples are now updated.
--> Now supports & uses SPI.transfer(*buf, count) along with the standard SPI.transfer(), speeding up the read/write of anything larger than an byte (AVR Boards only)
--> Reduces the program storage space in the uC's flash (AVR Boards)

To-do
--> Need to find a way to better identify _notPrevWritten();
--> Need to fix up eraseBlock32K, eraseBlock64K and eraseSector times
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.4.0 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						11.09.2016											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Bugs Squashed:
--> Fixed bug preventing writing to Address 0x00
--> Fixed bug in _notPrevWritten() which did did not perform a thorough enough check to see if a location had been previously written to.
--> Fixed errorchecking bug - it now works for all data types - not just byte/char as previously.
Enhancements & Optimizations:
--> Added a function 'error()' to enable users to check for errors generated at any point in their program.
--> Optimized writePage() so it doesn't depend on other functions and so runs faster than before.
--> Diagnostics.ino now outputs time taken for each function as a part of its and provides additional diagnostic data.
--> Now have a common set of private functions - _prep(), _beginSPI(), _nextByte() & _endSPI() - to replace _prepWrite(), _prepRead(), _beginRead(), _readNextByte(), _beginWrite(), _writeNextByte() & _endProcess();
--> Changed the way _addressCheck() works. It now checks all addresses involved in writing the data rather than one address block at a time and returns the correct address to write to in case overflow is enabled. Also, this function is now built into _prep().
--> Reading and writing using page numbers + offset has been optimised to be faster than before.
--> Optimized the way address and error checking is done so its more efficient and uses few system resources and runs faster.
--> Using this library with an ESP8266 board defaults to using GPIO15 as the Slave Select - unless something different is explicitly specified in the constructor.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.3.1 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						19.06.2016											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> W25Q256FV support added. (Thanks Stanislav-Povolotsky!)
--> Cleaned up redundant code in TestFlash.ino and SPIFlash.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.3.0 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						04.06.2016											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> ESP8266 compatibility added
--> Compatible with Arduino Fio and Micro
--> Now compatible with Arduino IDE v1.6.9
--> Compatible with Arduino-Makefile. (Thanks Raphael!)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.2.0 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						24.11.2015											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> When #RUNDIAGNOSTIC is uncommented, the _troubleshoot() function is now optimised for different µCs
--> Added the ability to check if the address has been previously written to before initiating a write
	operation.
--> Added a sizeofStr() function to get sizes of String objects, to use with the getAddress() function
--> Fixed a bug with getAddress()
--> Added the ability get the chip's name via getChipName()
--> Diagnostics.ino has been made more and efficient and provides a cleaner Serial output
--> Added getAddressEx.ino to show how getAdress() works.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.1.1 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						24.10.2015											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> Fixed bugs that prevented the following functions from working properly
			A. powerDown()
			B. powerUp()
			C. suspendProg()
			D. resumeProg()

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.1.0 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						18.10.2015											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> Arduino Due compatible - refer to wiki for further details
--> Fixed bug with write/readByteArray
--> Added write/readCharArray
--> Added a proper error checking function that gets called when #ifdef RUNDIAGNOSTIC is uncommented in SPIFlash.cpp.
	This function returns a verbose error message to the Serial console instead of the terse error codes of the previous version.
--> The following functions have been changed to enable bug fixes and uniformity in coding style.
			A. writeBytes()
			B. readBytes()
			They have been replaced with readByteArray() and writeByteArray().

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 2.0.0 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						12.10.2015											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> Sped up all functions atleast 25x
--> Compatible with ATTiny85
--> All Read/Write/Erase functions can now take either (page number & offset) or (address) as arguments
	(Except readPage() & printPage())
--> getAddress() can now return either a 32-bit address or a page number & offset - Refer to Readme.md
--> Error codes explained:
		0x00	SUCCESS 		Operation successful.
		0x01	CALLBEGIN		Please make sure .begin() has been called in setup().
		0x02	UNKNOWNCHIP		Unknown chip manufacturer.
		0x03	UNKNOWNCAP		Unknown chip capacity.
		0x04	CHIPBUSY		Chip busy.
		0x05	OUTOFBOUNDS		Address out of bounds. Please check if .begin() has been called in setup().
		0x06	CANTENWRITE		Unable to _writeEnable. Check wiring/chip.
		0x07	OUTOFMEM		Pagenumber outside maximum.
		0x08	OUTOFPAGE		Offset is outside page.
--> The following functions are deprecated to enable compatibility with other AVR chips.

			A. _printPageBytes()
			B. printPage()
			C. printAllPages()
			D. readSerialStr()

	They can be used by uncommenting them in the SPIFlash.cpp file. However, be warned, this particular block of code has only
	been tested with the Arduino	IDE (1.6.5) and only with 8-bit AVR based Arduino boards and will not be supported any further.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//				 	 						Version 1.3.2 									   	   //
//									Author: Prajwal Bhattaram									   //
//					  						09.10.2015											   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

--> Added the ability to read and write String objects with a simple high level function readStr() & writeStr()
--> Added the ability to getAddress()
--> Added the ability to fastRead to every read function as the last boolean argument (defaults to FALSE)
--> Changed the example code as given below:
		--> Modified TestFlash.ino to use 25% less memory
		--> Struct_writer.ino now writes struct to a random location on the Flash Memory chip.
		--> Added instructions for real world data storage to Struct_writer.ino
		--> Diagnostics.ino now provides a cleaner diagnostic readout
