#include<SPIFlash.h>
#include<SPI.h>

const int CSPin = 8;
boolean commandReady(false);
String command;
uint8_t pageBuffer[256];

SPIFlash flash(CSPin);

void setup() {
  Serial.begin(9600);
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i = 10; ++i)
  {
    Serial.print(F("."));
  }
  commandList();
}

void loop() {
  if (commandReady) {
    if (command == "get_ID") {
      flash.getID();
    }

    else if (command == "erase_chip") {
      flash.eraseChip();
    }

    else if (command == "read_all_pages") {
      flash.readAllPages();
    }

    //One parameter commands
    else if (command.startsWith("read_page")) {
      int pos = command.indexOf(" ");
      if (pos == -1) {
        Serial.println(F("Error: Command 'read_page' expects a page number"));
      }
      else {
        uint16_t page = (uint16_t)command.substring(pos).toInt();
        flash.readPage(page, pageBuffer);
      }
    }

    else if (command.startsWith("write_page")) {
      int pos = command.indexOf(" ");
      if (pos == -1) {
        Serial.println(F("Error: Command 'write_page' expects a page number"));
      }
      else {
        uint16_t page = (uint16_t)command.substring(pos).toInt();

        for (byte i = 0; i < 256; ++i) {
          pageBuffer[i] = i;
        }

        flash.writePage(page, pageBuffer);
      }
    }

    // Two parameter command
    else if (command.startsWith("read_byte")) {
      uint16_t pageno;
      uint8_t offset;

      String var[2];
      for (int i = 0; i < 2; ++i) {
        int pos = command.indexOf(" ");
        if (pos == -1) {
          Serial.println(F("Syntax error in read_byte"));
          goto done;
        }
        var[i] = command.substring(pos + 1);
        command = var[i];
      }
      pageno = (uint16_t)var[0].toInt();
      offset = (uint8_t)var[1].toInt();
      uint8_t data = flash.readByte(pageno, offset);
    }
    
    // Three parameter command
    else if (command.startsWith("write_byte")) {
      uint16_t pageno;
      uint8_t offset;
      uint8_t data;

      String var[3];
      for (int i = 0; i < 3; ++i) {
        int pos = command.indexOf(" ");
        if (pos == -1) {
          Serial.println(F("Syntax error in write_byte"));
          goto done;
        }
        var[i] = command.substring(pos + 1);
        command = var[i];
      }
      pageno = (uint16_t)var[0].toInt();
      offset = (uint8_t)var[1].toInt();
      data = (uint8_t)var[2].toInt();
      flash.writeByte(pageno, offset, data);
    }
    else {
      Serial.print(F("Invalid command sent: "));
      Serial.println(command);
    }
done:
    command = "";
    commandReady = false;
    commandList();
  }
}

/*
 * This handy built-in callback function alerts the UNO
 * whenever a serial event occurs. In our case, we check
 * for available input data and concatenate a command
 * string, setting a boolean used by the loop() routine
 * as a dispatch trigger.
 */
void serialEvent() {
  char c;
  while (Serial.available()) {
    c = (char)Serial.read();
    if (Serial.read() == '\n') {
      commandReady = true;
    }
    else {
      command += c;
    }
  }
}

void commandList() {
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("                                                W25Q80BV - Winbond 64 Mbit (1MB) Flash                                            "));
  Serial.println(F("                                                        SPIFlash library test                                                     "));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------"));
  Serial.println(F("  # Please pick from the following commands and type it into the Serial console #"));
  Serial.println(F("  1. get_ID"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'get_ID' gets the JEDEC ID of the chip as a 32 bit number"));
  Serial.println(F("  2. write_byte [page] [offset] [byte]"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'write_byte 100 20 224' writes the byte 224 to page 100 position 20"));
  Serial.println(F("  3. read_byte [page] [offset]"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'read_byte 100 20' returns the byte from page 100 position 20"));
  Serial.println(F("  4. write_page [page]"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'write_page 33' writes bytes from 0 to 255 sequentially to fill page 33"));
  Serial.println(F("  5. read_page [page]"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'write_page 33' writes bytes from 0 to 255 sequentially to fill page 33"));
  Serial.println(F("  6. read_all_pages"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'read_all_pages' reads all 4096 pages and outputs them to the serial console"));
  Serial.print(F("\t\t"));
  Serial.println(F("This function is to extract data from a flash chip onto a computer as a text file. Refer to 'Read me.md' in the library for details"));
  Serial.println(F("  7. erase_chip"));
  Serial.print(F("\t\t"));
  Serial.println(F("For example: 'erase_chip' erases the entire chip"));
}