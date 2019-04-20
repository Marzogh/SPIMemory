/*
  ----------------------------------------------------------------------------------------------------------------------------------
  |                                                            Winbond Flash                                                         |
  |                                                      SPIMemory library test v3.0.1                                                |
  |----------------------------------------------------------------------------------------------------------------------------------|
  |                                                                Marzogh                                                           |
  |                                                              16.11.2016                                                          |
  |                                                          Modified: hanyazou                                                      |
  |                                                              19.11.2017                                                          |
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
  |  2. writeByte [address] [byte]                                                                                                   |
  |   '2' followed by '2435' and then by '224' writes the byte 224 to address 2435                                                   |
  |                                                                                                                                  |
  |  3. readByte [address]                                                                                                           |
  |   '3' followed by '2435' returns the byte from address '2435'                                                                    |
  |                                                                                                                                  |
  |  4. writeWord [address] [word]                                                                                                   |
  |   '4' followed by '5948' and then by '633' writes the int 633 to address 5948                                                    |
  |                                                                                                                                  |
  |  5. readWord [address]                                                                                                           |
  |   '5' followed by '5948' returns the int from address 5948                                                                       |
  |                                                                                                                                  |
  |  6. writeStr [address] [inputString]                                                                                             |
  |   '6' followed by '345736' and then by 'Test String 1!' writes the String 'Test String 1! to address 345736                      |
  |                                                                                                                                  |
  |  7. readStr [address] [outputString]                                                                                             |
  |   '7' followed by '345736' reads the String from address 345736 into the outputString                                            |
  |                                                                                                                                  |
  |  8. writePage [page]                                                                                                             |
  |   '8' followed by '33' writes bytes from 0 to 255 sequentially to fill a page (256 bytes) starting with address 33               |
  |                                                                                                                                  |
  |  9. printPage [page]                                                                                                             |
  |   '9' followed by 33 reads & prints a page (256 bytes) starting with address 33. To just read a page to a data buffer, refer     |
  |    to 'ReadMe.md' in the library folder.                                                                                         |
  |                                                                                                                                  |
  |  10. printAllData                                                                                                                |
  |   '10' reads the entire chip and outputs the data as a byte array to the serial console                                          |
  |   This function is to extract data from a flash chip onto a computer as a text file.                                             |
  |   Refer to 'Read me.md' in the library for details.                                                                              |
  |                                                                                                                                  |
  |  11. Erase 4KB sector                                                                                                            |
  |   '11'  followed by 2 erases a 4KB sector containing the address be erased                                                       |
  |   Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255                                            |
  |                                                                                                                                  |
  |  12. Erase 32KB block                                                                                                            |
  |   '12'  followed by 2 erases a 32KB block containing the address to be erased                                                    |
  |   Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255                                            |
  |                                                                                                                                  |
  |  13. Erase 64KB block                                                                                                            |
  |   '13'  followed by 2 erases a 64KB block containing the address to be erased                                                    |
  |   Page 0-15 --> Sector 0; Page 16-31 --> Sector 1;......Page 4080-4095 --> Sector 255                                            |
  |                                                                                                                                  |
  |  14. Erase Chip                                                                                                                  |
  |   '14' erases the entire chip                                                                                                    |
  |                                                                                                                                  |
  ^----------------------------------------------------------------------------------------------------------------------------------^
*/



#include<SPIMemory.h>
uint8_t pageBuffer[256];
String serialCommand;
char printBuffer[128];
uint32_t addr;
uint8_t dataByte;
uint16_t dataInt;
String inputString, outputString;

//Define a flash memory size (if using non-Winbond memory) according to the list in defines.h
//#define CHIPSIZE MB64

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#if defined (SIMBLEE)
#define BAUD_RATE 250000
#define RANDPIN 1
#else
#define BAUD_RATE 115200
#if defined(ARCH_STM32)
#define RANDPIN PA0
#else
#define RANDPIN A0
#endif
#endif

SPIFlash flash;                   
//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus

void setup() {
  
  Serial.begin(BAUD_RATE);
#if defined (ARDUINO_ARCH_SAMD) || (__AVR_ATmega32U4__) || defined(ARCH_STM32) || defined(NRF5)
  while (!Serial) ; // Wait for Serial monitor to open
#endif
  delay(50); //Time to terminal get connected
  Serial.print(F("Initialising"));
  for (uint8_t i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  randomSeed(analogRead(RANDPIN));
  flash.begin();
  //To use a custom flash memory size (if using memory from manufacturers not officially supported by the library) - declare a size variable according to the list in defines.h
  //flash.begin(MB(1));
  
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
      uint8_t b1, b2, b3;
      uint32_t JEDEC = flash.getJEDECID();
      //uint16_t ManID = flash.getManID();
      b1 = (JEDEC >> 16);
      b2 = (JEDEC >> 8);
      b3 = (JEDEC >> 0);
      clearprintBuffer();
      sprintf(printBuffer, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh", b1, b2, b3);
      Serial.println(printBuffer);
      clearprintBuffer();
      sprintf(printBuffer, "JEDEC ID: %04xh", JEDEC);
      Serial.println(printBuffer);
      printLine();
      printNextCMD();
    }
    else if (commandNo == 2) {
      printLine();
      Serial.println(F("                                                       Function 2 : Write Byte                                                    "));
      printSplash();
      printLine();
      inputAddress();
      Serial.print(F("Please enter the value of the byte (0-255) you wish to save: "));
      while (!Serial.available()) {
      }
      dataByte = Serial.parseInt();
      Serial.println(dataByte);
      if (flash.writeByte(addr, dataByte)) {
        clearprintBuffer();
        sprintf(printBuffer, "%d has been written to address %d", dataByte, addr);
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
      inputAddress();
      clearprintBuffer();
      sprintf(printBuffer, "The byte at address %d is: ", addr);
      Serial.print(printBuffer);
      Serial.println(flash.readByte(addr));
      printLine();
      printNextCMD();
    }
    else if (commandNo == 4) {
      printLine();
      Serial.println(F("                                                       Function 4 : Write Word                                                    "));
      printSplash();
      printLine();
      inputAddress();
      Serial.print(F("Please enter the value of the word (>255) you wish to save: "));
      while (!Serial.available()) {
      }
      dataInt = Serial.parseInt();
      Serial.println(dataInt);
      if (flash.writeWord(addr, dataInt)) {
        clearprintBuffer();
        sprintf(printBuffer, "%d has been written to address %d", dataInt, addr);
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
      inputAddress();
      clearprintBuffer();
      sprintf(printBuffer, "The unsigned int at address %d is: ", addr);
      Serial.print(printBuffer);
      Serial.println(flash.readWord(addr));
      printLine();
      printNextCMD();
    }
    else if (commandNo == 6) {
      printLine();
      Serial.println(F("                                                      Function 6 : Write String                                                   "));
      printSplash();
      printLine();
      Serial.println(F("This function will write a String of your choice to the page selected."));
      inputAddress();
      Serial.println(F("Please enter the String you wish to save: "));
      while (!Serial.available()) {
      }
      readSerialStr(inputString);
      if (flash.writeStr(addr, inputString)) {
        clearprintBuffer();
        Serial.print(F("String '"));
        Serial.print(inputString);
        sprintf(printBuffer, "' has been written to address %d", addr);
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
      Serial.print(F("This function will read a string from your address of choice: "));
      inputAddress();
      clearprintBuffer();
      sprintf(printBuffer, "The String at address %d is: ", addr);
      Serial.print(printBuffer);
      flash.readStr(addr, outputString);
      Serial.println(outputString);
      printLine();
      printNextCMD();
    }
    else if (commandNo == 8) {
      printLine();
      Serial.println(F("                                                       Function 8 : Write Page                                                    "));
      printSplash();
      printLine();
      Serial.println(F("This function will write a sequence of bytes (0-255) starting from your address of choice"));
      Serial.print(F("Please enter the address you wish to write to: "));
      while (!Serial.available()) {
      }
      addr = Serial.parseInt();
      Serial.println(addr);
      for (uint16_t i = 0; i < SPI_PAGESIZE; ++i) {
        pageBuffer[i] = i;
      }
      if (flash.writeByteArray(addr, &pageBuffer[0], SPI_PAGESIZE)) {
        clearprintBuffer();
        sprintf(printBuffer, "Values from 0 to 255 have been written starting from the address %d", addr);
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
          printPage(addr, outputType);
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
      Serial.println(F("This function will read 256 bytes from the address selected."));
      Serial.print(F("Please enter the address you wish to read: "));
      while (!Serial.available()) {
      }
      addr = Serial.parseInt();
      Serial.println(addr);
      printOutputChoice();
      while (!Serial.available()) {
      }
      uint8_t outputType = Serial.parseInt();
      Serial.println(outputType);
      printPage(addr, outputType);
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
      Serial.print(F("Please enter the address you wish to erase: "));
      while (!Serial.available()) {
      }
      addr = Serial.parseInt();
      Serial.println(addr);
      if (flash.eraseSector(addr)) {
        clearprintBuffer();
        sprintf(printBuffer, "A 4KB sector containing address %d has been erased", addr);
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
          printPage(addr, outputType);
        }
      } else {
        Serial.println("Erasing sector failed");
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
      Serial.print(F("Please enter the address you wish to erase: "));
      while (!Serial.available()) {
      }
      addr = Serial.parseInt();
      Serial.println(addr);
      if (flash.eraseBlock32K(addr)) {
        clearprintBuffer();
        sprintf(printBuffer, "A 32KB block containing address %d has been erased", addr);
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
          printPage(addr, outputType);
        }
      } else {
        Serial.println("Erasing block 32K failed");
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
      Serial.print(F("Please enter the address you wish to erase: "));
      while (!Serial.available()) {
      }
      addr = Serial.parseInt();
      Serial.println(addr);
      if (flash.eraseBlock64K(addr)) {
        clearprintBuffer();
        sprintf(printBuffer, "A 64KB block containing address %d has been erased", addr);
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
          printPage(addr, outputType);
        }
      } else {
        Serial.println("Erasing block 64K failed");
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
void printPage(uint32_t _address, uint8_t outputType) {
  if (!Serial)
    Serial.begin(115200);

  char buffer[24];
  sprintf(buffer, "Reading address 0x(%04x)", _address);
  Serial.println(buffer);

  uint8_t data_buffer[SPI_PAGESIZE];
  flash.readByteArray(_address, &data_buffer[0], SPI_PAGESIZE);
  _printPageBytes(data_buffer, outputType);
}

//Reads all pages on Flash chip and dumps it to Serial stream.
//This function is useful when extracting data from a flash chip onto a computer as a text file.
void printAllPages(uint8_t outputType) {
  if (!Serial)
    Serial.begin(115200);

  Serial.println("Reading all pages");
  uint8_t data_buffer[256];

  uint32_t maxAddr = flash.getCapacity();
  for (int a = 0; a < maxAddr; a++) {
    flash.readByteArray(a, &data_buffer[0], 256);
    _printPageBytes(data_buffer, outputType);
    delay(100);
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

void inputAddress(void) {
  Serial.print(F("Please enter the address (0 - CAPACITY) you wish to access: "));
  while (!Serial.available()) {
  }
  addr = Serial.parseInt();
  Serial.println(addr);
}
