/*
 *----------------------------------------------------------------------------------------------------------------------------------*
 |                                                            Winbond Flash                                                         |
 |                                                      SPIFlash library test v2.2.0                                                |
 |----------------------------------------------------------------------------------------------------------------------------------|
 |                                                                Marzogh                                                           |
 |                                                              25.11.2015                                                          |
 |----------------------------------------------------------------------------------------------------------------------------------|
 |                                     (Please make sure your Serial monitor is set to 'No Line Ending')                            |
 |                                     *****************************************************************                            |
 |                                                                                                                                  |
 |                     # Please pick from the following commands and type the command number into the Serial console #              |
 |    For example - to write a byte of data, you would have to use the write_byte function - so type '3' into the serial console.   |
 |                                                    --------------------------------                                              |
 |                                                                                                                                  |
 |  1. getID                                                                                                                        |
 |   '1' gets the JEDEC ID of the chip                                                                                              |
 |                                                                                                                                  |
 |  2. writeByte [page] [offset] [byte]                                                                                             |
 |   '2' followed by '100' and then by '20' and then by '224' writes the byte 224 to page 100 position 20                           |
 |                                                                                                                                  |
 |  3. readByte [page] [offset]                                                                                                     |
 |   '3' followed by '100' and then by '20' returns the byte from page 100 position 20                                              |
 |                                                                                                                                  |
 |  4. writeWord [page] [offset]                                                                                                    |
 |   '4' followed by '55' and then by '35' and then by '633' writes the int 633 to page 5 position 35                               |
 |                                                                                                                                  |
 |  5. readWord [page] [offset]                                                                                                     |
 |   '5' followed by '200' and then by '30' returns the int from page 200 position 30                                               |
 |                                                                                                                                  |
 |  6. writeStr [page] [offset] [inputString]                                                                                       |
 |   '6' followed by '345' and then by '65' and then by 'Test String 1!' writes the String 'Test String 1! to page 345 position 65  |
 |                                                                                                                                  |
 |  7. readStr [page] [offset] [outputString]                                                                                       |
 |   '7' followed by '2050' and then by '73' reds the String from page 2050 position 73 into the outputString                       |
 |                                                                                                                                  |
 |  8. writePage [page]                                                                                                             |
 |   '8' followed by '33' writes bytes from 0 to 255 sequentially to fill page 33                                                   |
 |                                                                                                                                  |
 |  9. printPage [page]                                                                                                             |
 |   '9' followed by 33 reads & prints page 33. To just read a page to a data buffer, refer                                         |
 |    to 'ReadMe.md' in the library folder.                                                                                         |
 |                                                                                                                                  |
 |  10. printAllPages                                                                                                               |
 |   '10' reads all 4096 pages and outputs them to the serial console                                                               |
 |   This function is to extract data from a flash chip onto a computer as a text file.                                             |
 |   Refer to 'Read me.md' in the library for details.                                                                              |
 |                                                                                                                                  |
 |  11. Erase 4KB sector                                                                                                            |
 |   '11'  followed by 2 erases a 4KB sector containing the page to be erased                                                       |
 |   Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255                                            |
 |                                                                                                                                  |
 |  12. Erase 32KB block                                                                                                            |
 |   '12'  followed by 2 erases a 32KB block containing the page to be erased                                                       |
 |   Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255                                            |
 |                                                                                                                                  |
 |  13. Erase 64KB block                                                                                                            |
 |   '13'  followed by 2 erases a 64KB block containing the page to be erased                                                       |
 |   Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255                                            |
 |                                                                                                                                  |
 |  14. Erase Chip                                                                                                                  |
 |   '14' erases the entire chip                                                                                                    |
 |                                                                                                                                  |
 ^----------------------------------------------------------------------------------------------------------------------------------^
 */



#include<SPIFlash.h>
#include<SPI.h>
const int cs = 10;
uint8_t pageBuffer[256];
String serialCommand;
char printBuffer[128];
uint16_t page;
uint8_t offset, dataByte;
uint16_t dataInt;
String inputString, outputString;


SPIFlash flash(cs);

void setup() {
  Serial.begin(115200);
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  flash.begin();
  /*
   //Uncomment this code block if your code doesn't run to check for Flash error code
  clearprintBuffer();
  sprintf(printBuffer, "Flash initiation code: %x", flashCode);
  Serial.println(printBuffer);
  Serial.println(F("If flashCode !=0 please redownload the latest version of the library."));
  Serial.println(F("If flashcode still !=0 after downloading the latest version, please raise an issue at https://github.com/Marzogh/SPIFlash/issues."));
  */
  Serial.println();
  Serial.println();
  commandList();
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t commandNo = Serial.parseInt();
    if (commandNo == 0) {
      commandList();
    }
    else if (commandNo == 1) {
      printLine();
      Serial.println(F("                                                      Function 1 : Get JEDEC ID                                                   "));
      printLine();
      printLine();
      uint8_t b1, b2;
      uint16_t b3;
      uint32_t JEDEC = flash.getJEDECID();
      uint16_t ManID = flash.getManID();
      b1 = (ManID >> 8);
      b2 = (ManID >> 0);
      b3 = (JEDEC >> 0);
      clearprintBuffer();
      sprintf(printBuffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh", b1, b2, b3);
      Serial.println(printBuffer);
      clearprintBuffer();
      sprintf(printBuffer, "JEDEC ID: %04lxh", JEDEC);
      Serial.println(printBuffer);
      printLine();
      printNextCMD();
    }
    else if (commandNo == 2) {
      printLine();
      Serial.println(F("                                                       Function 2 : Write Byte                                                    "));
      printSplash();
      printLine();
      Serial.print(F("Please enter the number of the page you wish to modify: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      Serial.print(F("Please enter the position on the page (0-255) you wish to modify: "));
      while (!Serial.available()) {
      }
      offset = Serial.parseInt();
      Serial.println(offset);
      Serial.print(F("Please enter the value of the byte (0-255) you wish to save: "));
      while (!Serial.available()) {
      }
      dataByte = Serial.parseInt();
      Serial.println(dataByte);
      if (flash.writeByte(page, offset, dataByte)) {
        clearprintBuffer();
        sprintf(printBuffer, "%d has been written to position %d on page %d", dataByte, offset, page);
        Serial.println(printBuffer);
      }
      else {
        writeFail();
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 3) {
      printLine();
      Serial.println(F("                                                       Function 3 : Read Byte                                                     "));
      printSplash();
      printLine();
      Serial.print(F("Please enter the number of the page the byte you wish to read is on: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      Serial.print(F("Please enter the position of the byte on the page (0-255) you wish to read: "));
      while (!Serial.available()) {
      }
      offset = Serial.parseInt();
      Serial.println(offset);
      clearprintBuffer();
      sprintf(printBuffer, "The byte at position %d on page %d is: ", offset, page);
      Serial.print(printBuffer);
      Serial.println(flash.readByte(page, offset));
      printLine();
      printNextCMD();
    }
    else if (commandNo == 4) {
      printLine();
      Serial.println(F("                                                       Function 4 : Write Word                                                    "));
      printSplash();
      printLine();
      Serial.print(F("Please enter the number of the page you wish to modify: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      Serial.print(F("Please enter the position on the page (0-255) you wish to modify: "));
      while (!Serial.available()) {
      }
      offset = Serial.parseInt();
      Serial.println(offset);
      Serial.print(F("Please enter the value of the word (>255) you wish to save: "));
      while (!Serial.available()) {
      }
      dataInt = Serial.parseInt();
      Serial.println(dataInt);
      if (flash.writeWord(page, offset, dataInt)) {
        clearprintBuffer();
        sprintf(printBuffer, "%d has been written to position %d on page %d", dataInt, offset, page);
        Serial.println(printBuffer);
      }
      else {
        writeFail();
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 5) {
      printLine();
      Serial.println(F("                                                       Function 5 : Read Word                                                     "));
      printSplash();
      printLine();
      Serial.print(F("Please enter the number of the page the byte you wish to read is on: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      Serial.print(F("Please enter the position of the word on the page (0-255) you wish to read: "));
      while (!Serial.available()) {
      }
      offset = Serial.parseInt();
      Serial.println(offset);
      clearprintBuffer();
      sprintf(printBuffer, "The unsigned int at position %d on page %d is: ", offset, page);
      Serial.print(printBuffer);
      Serial.println(flash.readWord(page, offset));
      printLine();
      printNextCMD();
    }
    else if (commandNo == 6) {
      printLine();
      Serial.println(F("                                                      Function 6 : Write String                                                   "));
      printSplash();
      printLine();
      Serial.println(F("This function will write a String of your choice to the page selected."));
      Serial.print(F("Please enter the number of the page you wish to write to: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      Serial.print(F("Please enter the position on the page (0-255) you wish to write to: "));
      while (!Serial.available()) {
      }
      offset = Serial.parseInt();
      Serial.println(offset);
      Serial.println(F("Please enter the String you wish to save: "));
      while (!Serial.available()) {
      }
      readSerialStr(inputString);
      if (flash.writeStr(page, offset, inputString)) {
        clearprintBuffer();
        Serial.print(F("String '"));
        Serial.print(inputString);
        sprintf(printBuffer, "' has been written to position %d on page %d", offset, page);
        Serial.println(printBuffer);
      }
      else {
        writeFail();
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 7) {
      printLine();
      Serial.println(F("                                                      Function 7 : Read String                                                    "));
      printSplash();
      printLine();
      Serial.print(F("Please enter the number of the page the String you wish to read is on: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      Serial.print(F("Please enter the position of the String on the page (0-255) you wish to read: "));
      while (!Serial.available()) {
      }
      offset = Serial.parseInt();
      Serial.println(offset);
      clearprintBuffer();
      sprintf(printBuffer, "The String at position %d on page %d is: ", offset, page);
      Serial.print(printBuffer);
      flash.readStr(page, offset, outputString);
      Serial.println(outputString);
      printLine();
      printNextCMD();
    }
    else if (commandNo == 8) {
      printLine();
      Serial.println(F("                                                       Function 8 : Write Page                                                    "));
      printSplash();
      printLine();
      Serial.println(F("This function will write a sequence of bytes (0-255) to the page selected."));
      Serial.print(F("Please enter the number of the page you wish to write to: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      for (int i = 0; i < 256; ++i) {
        pageBuffer[i] = i;
      }
      if (flash.writePage(page, pageBuffer)) {
        clearprintBuffer();
        sprintf(printBuffer, "Values from 0 to 255 have been written to the page %d", page);
        Serial.println(printBuffer);
        printReadChoice();
        while (!Serial.available()) {
        }
        uint8_t choice = Serial.parseInt();
        Serial.println(choice);
        if (choice == 1) {
          printOutputChoice();
          while (!Serial.available()) {
          }
          uint8_t outputType = Serial.parseInt();
          Serial.println(outputType);
          printPage(page, outputType);
        }
      }
      else {
        writeFail();
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 9) {
      printLine();
      Serial.println(F("                                                       Function 9 : Read Page                                                    "));
      printSplash();
      printLine();
      Serial.println(F("This function will read the entire page selected."));
      Serial.print(F("Please enter the number of the page you wish to read: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      printOutputChoice();
      while (!Serial.available()) {
      }
      uint8_t outputType = Serial.parseInt();
      Serial.println(outputType);
      printPage(page, outputType);
      printLine();
      printNextCMD();
    }
    else if (commandNo == 10) {
      printLine();
      Serial.println(F("                                                     Function 10 : Read All Pages                                                  "));
      printSplash();
      printLine();
      Serial.println(F("This function will read the entire flash memory."));
      Serial.println(F("This will take a long time and might result in memory issues. Do you wish to continue? (Y/N)"));
      char c;
      while (!Serial.available()) {
      }
      c = (char)Serial.read();
      if (c == 'Y' || c == 'y') {
        printOutputChoice();
        while (!Serial.available()) {
        }
        uint8_t outputType = Serial.parseInt();
        Serial.println(outputType);
        printAllPages(outputType);
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 11) {
      printLine();
      Serial.println(F("                                                       Function 11 : Erase 4KB sector                                               "));
      printSplash();
      printLine();
      Serial.println(F("This function will erase a 4KB sector."));
      Serial.print(F("Please enter the number of the page you wish to erase: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      flash.eraseSector(page, 0);
      clearprintBuffer();
      sprintf(printBuffer, "A 4KB sector containing page %d has been erased", page);
      Serial.println(printBuffer);
      printReadChoice();
      while (!Serial.available()) {
      }
      uint8_t choice = Serial.parseInt();
      Serial.println(choice);
      if (choice == 1) {
        printOutputChoice();
        while (!Serial.available()) {
        }
        uint8_t outputType = Serial.parseInt();
        Serial.println(outputType);
        printPage(page, outputType);
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 12) {
      printLine();
      Serial.println(F("                                                       Function 12 : Erase 32KB Block                                              "));
      printSplash();
      printLine();
      Serial.println(F("This function will erase a 32KB block."));
      Serial.print(F("Please enter the number of the page you wish to erase: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      flash.eraseBlock32K(page, 0);
      clearprintBuffer();
      sprintf(printBuffer, "A 32KB block containing page %d has been erased", page);
      Serial.println(printBuffer);
      printReadChoice();
      while (!Serial.available()) {
      }
      uint8_t choice = Serial.parseInt();
      Serial.println(choice);
      if (choice == 1) {
        printOutputChoice();
        while (!Serial.available()) {
        }
        uint8_t outputType = Serial.parseInt();
        Serial.println(outputType);
        printPage(page, outputType);
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 13) {
      printLine();
      Serial.println(F("                                                       Function 13 : Erase 64KB Block                                              "));
      printSplash();
      printLine();
      Serial.println(F("This function will erase a 64KB block."));
      Serial.print(F("Please enter the number of the page you wish to erase: "));
      while (!Serial.available()) {
      }
      page = Serial.parseInt();
      Serial.println(page);
      flash.eraseBlock64K(page, 0);
      clearprintBuffer();
      sprintf(printBuffer, "A 64KB block containing page %d has been erased", page);
      Serial.println(printBuffer);
      printReadChoice();
      while (!Serial.available()) {
      }
      uint8_t choice = Serial.parseInt();
      Serial.println(choice);
      if (choice == 1) {
        printOutputChoice();
        while (!Serial.available()) {
        }
        uint8_t outputType = Serial.parseInt();
        Serial.println(outputType);
        printPage(page, outputType);
      }
      printLine();
      printNextCMD();
    }
    else if (commandNo == 14) {
      printLine();
      Serial.println(F("                                                      Function 14 : Erase Chip                                                    "));
      printSplash();
      printLine();
      Serial.println(F("This function will erase the entire flash memory."));
      Serial.println(F("Do you wish to continue? (Y/N)"));
      char c;
      while (!Serial.available()) {
      }
      c = (char)Serial.read();
      if (c == 'Y' || c == 'y') {
        if (flash.eraseChip())
          Serial.println(F("Chip erased"));
        else
          Serial.println(F("Error erasing chip"));
      }
      printLine();
      printNextCMD();
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void clearprintBuffer()
{
  for (uint8_t i = 0; i < 128; i++) {
    printBuffer[i] = 0;
  }
}

//Reads a string from Serial
bool readSerialStr(String &inputStr) {
  if (!Serial)
    Serial.begin(115200);
  while (Serial.available()) {
    inputStr = Serial.readStringUntil('\n');
    return true;
  }
  return false;
}

//Prints hex/dec formatted data from page reads - for debugging
void _printPageBytes(uint8_t *data_buffer, uint8_t outputType) {
  char buffer[10];
  for (int a = 0; a < 16; ++a) {
    for (int b = 0; b < 16; ++b) {
      if (outputType == 1) {
        sprintf(buffer, "%02x", data_buffer[a * 16 + b]);
        Serial.print(buffer);
      }
      else if (outputType == 2) {
        uint8_t x = data_buffer[a * 16 + b];
        if (x < 10) Serial.print("0");
        if (x < 100) Serial.print("0");
        Serial.print(x);
        Serial.print(',');
      }
    }
    Serial.println();
  }
}

//Reads a page of data and prints it to Serial stream. Make sure the sizeOf(uint8_t data_buffer[]) == 256.
void printPage(uint16_t page_number, uint8_t outputType) {
  if (!Serial)
    Serial.begin(115200);

  char buffer[24];
  sprintf(buffer, "Reading page (%04x)", page_number);
  Serial.println(buffer);

  uint8_t data_buffer[256];
  flash.readPage(page_number, data_buffer);
  _printPageBytes(data_buffer, outputType);
}

//Reads all pages on Flash chip and dumps it to Serial stream.
//This function is useful when extracting data from a flash chip onto a computer as a text file.
void printAllPages(uint8_t outputType) {
  if (!Serial)
    Serial.begin(115200);

  Serial.println("Reading all pages");
  uint8_t data_buffer[256];

  uint32_t maxPage = flash.getMaxPage();
  for (int a = 0; a < maxPage; a++) {
    flash.readPage(a, data_buffer);
    _printPageBytes(data_buffer, outputType);
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Print commands~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void printLine()
{
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
}

void printSplash()
{
  Serial.println(F("                                                        SPIFlash library test                                                     "));
}

void printNextCMD()
{
  Serial.println(F("Please type the next command. Type 0 to get the list of commands"));
}

void printOutputChoice()
{
  Serial.print("Would you like your output in decimal or hexadecimal? Please indicate with '1' for HEX or '2' for DEC: ");
}

void printReadChoice()
{
  Serial.print("Type 1 to read the page you have just modified. Type 0 to continue: ");
}

void writeSuccess()
{
  Serial.println("Data write successful");
}

void writeFail()
{
  Serial.println("Data write failed");
}

