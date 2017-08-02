/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                               Struct_writer.ino                                                               |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.6.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  16.04.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                        This program writes a struct to a random location on your flash memory chip and reads it back.                         |
  |        Uncomment #define SENSOR below to get real world readings. Real world readings require a Light dependant resistor hooked up to A0.     |
  |                   For information on how to hook up an LDR to an Arduino, please refer to Adafruit's excellent tutorial at                    |
  |                                          https://learn.adafruit.com/photocells/using-a-photocell                                              |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include<SPIFlash.h>

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

#if defined (SIMBLEE)
#define BAUD_RATE 250000
#else
#define BAUD_RATE 115200
#endif

/*
   Uncomment the #define below if you would like real world readings.
   For real world readings, hook up a light dependant resistor to A1.

*/
//#define SENSOR
#if defined (SIMBLEE)
#define BAUD_RATE 250000
#define LDR 1
#else
#define BAUD_RATE 115200
#define LDR A0
#endif



SPIFlash flash;


struct Configuration {
  float lux;
  float vOut;                   // Voltage ouput from potential divider to Analog input
  float RLDR;                   // Resistance calculation of potential divider with LDR
  bool light;
  uint8_t adc;
};
Configuration configuration;

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


  uint32_t _addr = random(0, 1677215);

#ifndef SENSOR
  configuration.lux = 98.43;
  configuration.vOut = 4.84;
  configuration.RLDR = 889.32;
  configuration.light = true;
  configuration.adc = 5;
#endif

#ifdef SENSOR
  readLDR();
#endif

  if (flash.writeAnything(_addr, configuration))
    Serial.println ("Data write successful");
  else
    Serial.println ("Data write failed");

  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);

  Serial.println("Saved!");
  configuration.lux = 0;
  configuration.vOut = 0;
  configuration.RLDR = 0;
  configuration.light = 0;
  configuration.adc = 0;
  Serial.println();
  Serial.println("Local values set to 0");
  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);
  Serial.println();
  flash.readAnything(_addr, configuration);
  flash.eraseSector(_addr, 0);

  Serial.println("After reading");
  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);

}

void loop() {
  delay(1000);
}

#ifdef SENSOR
void readLDR()
{
  configuration.adc = analogRead(LDR);
  configuration.vOut = (configuration.adc * 0.0048828125);                       // vOut = Output voltage from potential Divider. [vOut = ADC * (Vin / 1024)]
  configuration.RLDR = (10.0 * (5 - configuration.vOut)) / configuration.vOut;   // Equation to calculate Resistance of LDR, [R-LDR =(R1 (Vin - vOut))/ vOut]. R1 is in KOhms
  // R1 = 10 KOhms , Vin = 5.0 Vdc.
  configuration.lux = (500 / configuration.RLDR);
}
#endif
