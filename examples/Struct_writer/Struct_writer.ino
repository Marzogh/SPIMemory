/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                               Struct_writer.ino                                                               |
  |                                                               SPIMemory library                                                                |
  |                                                                   v 3.1.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  03.03.2018                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program writes a struct to a random location on your flash memory chip and reads it back.                         |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include<SPIMemory.h>

//#define PRINTDETAIL
//#define PRINTINDIVIDUALRUNS
#define NUMBEROFREPEATS 100

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#if defined (SIMBLEE)
#define BAUD_RATE 250000
#define LDR 1
#else
#define BAUD_RATE 115200
#define LDR A0
#endif



//SPIFlash flash(SS1, &SPI1);       //Use this constructor if using an SPI bus other than the default SPI. Only works with chips with more than one hardware SPI bus
SPIFlash flash;

struct ConfigurationIn {
  float lux = 3.24;
  float vOut = 4.45;                    // Voltage ouput from potential divider to Analog input
  float RLDR = 1.234;                   // Resistance calculation of potential divider with LDR
  bool light = true;
  uint8_t adc = 45;
  uint8_t arr[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  struct MISC {
    byte tempHigh = 30;
    byte tempLow = 20;
    bool parkingMode = false;
    bool allowDataToBeSent = false;
  } misc;
  struct NETWORK {
    char ssid[5] = "ssid";
    char pwd[4] = "pwd";
    char userid[7] = "userid";
  } network;
  struct CHARGING_INFO {
    byte interval = 5;
    byte highChargingDefault = 80;
  } charging;
};
ConfigurationIn configurationIn;

struct ConfigurationOut {
  float lux;
  float vOut;                   // Voltage ouput from potential divider to Analog input
  float RLDR;                   // Resistance calculation of potential divider with LDR
  bool light;
  uint8_t adc;
  uint8_t arr[8];
  struct MISC {
    byte tempHigh;
    byte tempLow;
    bool parkingMode;
    bool allowDataToBeSent;
  } misc;
  struct NETWORK {
    char ssid[5];
    char pwd[4];
    char userid[7];
  } network;
  struct CHARGING_INFO {
    byte interval;
    byte highChargingDefault;
  } charging;
};
ConfigurationOut configurationOut;
uint16_t eraseCount, writeCount, errorCount, readCount;

void setup() {
  Serial.begin(BAUD_RATE);
  eraseCount = 0;
  writeCount = 0;
  errorCount = NUMBEROFREPEATS;
  readCount = 0;
#if defined (ARDUINO_SAMD_ZERO) || (__AVR_ATmega32U4__)
  while (!Serial) ; // Wait for Serial monitor to open
#endif
  randomSeed(analogRead(LDR));
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  flash.begin();
  Serial.println();
#ifdef PRINTINDIVIDUALRUNS
  printLine();
  Serial.println();
  Serial.print("\tStruct number\t\t\tSection Erase\t\t\tStruct Write\t\t\tErrorCheck\t\t\tStruct Read\t\t\t");
  Serial.println();
  printLine();
  Serial.println();
#endif

  for (uint16_t x = 1; x <= NUMBEROFREPEATS; x++) {
    Serial.println(x);
    //uint32_t _addr = random(0, 1677215);
    uint32_t _addr = random(0, flash.getCapacity());
#ifdef PRINTINDIVIDUALRUNS
    Serial.print("\t\t");
    Serial.print(x);
#endif

    if (flash.eraseSection(_addr, sizeof(configurationIn))) {
      //if (flash.eraseSector(_addr)) {
      eraseCount++;
#ifdef PRINTINDIVIDUALRUNS
      Serial.print("\t\t\t    Done");
#endif
    }
    else {
      eraseCount--;
    }
    /*if (flash.eraseSector(_addr)) {
      Serial.println("Sector has been erased");
      }*/
    if (flash.writeAnything(_addr, configurationIn)) {
#ifdef PRINTDETAIL
      Serial.println(configurationIn.lux);
      Serial.println(configurationIn.vOut);
      Serial.println(configurationIn.RLDR);
      Serial.println(configurationIn.light);
      Serial.println(configurationIn.adc);
      for (uint8_t i = 0; i < 8; i++) {
        Serial.print(configurationIn.arr[i]);
        Serial.print(",");
      }
      Serial.println();
      Serial.println(configurationIn.misc.tempHigh);
      Serial.println(configurationIn.misc.tempLow);
      Serial.println(configurationIn.misc.parkingMode);
      Serial.println(configurationIn.misc.allowDataToBeSent);
      for (uint8_t i = 0; i < 4; i++) {
        Serial.print(configurationIn.network.ssid[i]);
        Serial.print(",");
      }
      Serial.println();
      for (uint8_t i = 0; i < 3; i++) {
        Serial.print(configurationIn.network.pwd[i]);
        Serial.print(",");
      }
      Serial.println();
      for (uint8_t i = 0; i < 6; i++) {
        Serial.print(configurationIn.network.userid[i]);
        Serial.print(",");
      }
      Serial.println();
      Serial.println(configurationIn.charging.interval);
      Serial.println(configurationIn.charging.highChargingDefault);
#endif
      writeCount++;
      errorCount--;
#ifdef PRINTINDIVIDUALRUNS
      Serial.print("\t\t\t   Done\t\t\t\t   Pass");
#endif
    }
    else {
#ifdef PRINTINDIVIDUALRUNS
      Serial.print("\t\t\t   Done\t\t\t\t   Fail");
#endif
    }
delay(50);
    if (flash.readAnything(_addr, configurationOut)) {
#ifdef PRINTDETAIL
      Serial.println(configurationOut.lux);
      Serial.println(configurationOut.vOut);
      Serial.println(configurationOut.RLDR);
      Serial.println(configurationOut.light);
      Serial.println(configurationOut.adc);
      for (uint8_t i = 0; i < 8; i++) {
        Serial.print(configurationIn.arr[i]);
        Serial.print(",");
      }
      Serial.println();
      Serial.println(configurationOut.misc.tempHigh);
      Serial.println(configurationOut.misc.tempLow);
      Serial.println(configurationOut.misc.parkingMode);
      Serial.println(configurationOut.misc.allowDataToBeSent);
      for (uint8_t i = 0; i < 4; i++) {
        Serial.print(configurationOut.network.ssid[i]);
        Serial.print(",");
      }
      Serial.println();
      for (uint8_t i = 0; i < 3; i++) {
        Serial.print(configurationOut.network.pwd[i]);
        Serial.print(",");
      }
      Serial.println();
      for (uint8_t i = 0; i < 6; i++) {
        Serial.print(configurationOut.network.userid[i]);
        Serial.print(",");
      }
      Serial.println();
      Serial.println(configurationOut.charging.interval);
      Serial.println(configurationOut.charging.highChargingDefault);
#endif
      if (configurationIn.lux == configurationOut.lux || configurationIn.vOut == configurationOut.vOut || configurationIn.RLDR == configurationOut.RLDR || configurationIn.light == configurationOut.light || \
          configurationIn.adc == configurationOut.adc || configurationIn.arr[2] == configurationOut.arr[2] || configurationIn.misc.tempHigh == configurationOut.misc.tempHigh || configurationIn.misc.tempLow == configurationOut.misc.tempLow || \
          configurationIn.misc.parkingMode == configurationOut.misc.parkingMode || configurationIn.misc.allowDataToBeSent == configurationOut.misc.allowDataToBeSent || configurationIn.network.ssid[3] == configurationOut.network.ssid[3] || configurationIn.network.pwd[1] == configurationOut.network.pwd[1] || \
          configurationIn.network.userid[4] == configurationOut.network.userid[4] || configurationIn.charging.interval == configurationOut.charging.interval || configurationIn.charging.highChargingDefault == configurationOut.charging.highChargingDefault) {
#ifdef PRINTINDIVIDUALRUNS
        Serial.print("\t\t\t\t   Pass");
      Serial.println();
#endif
        readCount++;
      }
      else {
#ifdef PRINTINDIVIDUALRUNS
        Serial.print("\t\t\t\t   Fail");
      Serial.println();
#endif
        readCount--;
      }
    }
  }
#ifdef PRINTINDIVIDUALRUNS
  printLine();
#endif
  Serial.println();
  printLine();
  Serial.println();
  Serial.println("\t\tFinal results");
  printLine();
  Serial.println();
  Serial.print("\t\tNo. of successful erases: ");
  Serial.print("\t\t");
  Serial.println(eraseCount);
  Serial.print("\t\tNo. of successful writes: ");
  Serial.print("\t\t");
  Serial.println(writeCount);
  Serial.print("\t\tNo. of errors generated:");
  Serial.print("\t\t");
  Serial.print(errorCount);
  Serial.println("\t(errorCheck function failures)");
  Serial.print("\t\tNo. of successful reads: ");
  Serial.print("\t\t");
  Serial.println(readCount);
  printLine();
  Serial.println();
}

void loop() {
}

void printLine() {
  for (uint16_t i = 0; i < 160; i++) {
    Serial.print("-");
  }
}
