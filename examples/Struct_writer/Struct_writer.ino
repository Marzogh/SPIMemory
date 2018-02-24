/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                               Struct_writer.ino                                                               |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.6.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  24.02.2018                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program writes a struct to a random location on your flash memory chip and reads it back.                         |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include<SPIFlash.h>

//#define PRINTDETAIL

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
  float vOut = 4.45;                   // Voltage ouput from potential divider to Analog input
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


void setup() {
  Serial.begin(BAUD_RATE);
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
  Serial.println();
  flash.begin();

  for (uint8_t x = 1; x <= 20; x++) {
    //uint32_t _addr = random(0, 1677215);
    uint32_t _addr = random(0, flash.getCapacity());
    Serial.print("Size of array: ");
    Serial.println(sizeof(configurationIn));

    if (flash.eraseSection(_addr, sizeof(configurationIn))) {
      Serial.println("Section has been erased");
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
      Serial.print ("Data write ");
      Serial.print(x);
      Serial.println(" successful");
    }
    else {
      Serial.print ("Data write ");
      Serial.print(x);
      Serial.println(" failed");
    }
    Serial.println();

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
    }
  }
  delay(1000);
}

void loop() {
}
