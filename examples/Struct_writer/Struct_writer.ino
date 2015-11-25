/*
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                               Struct_writer.ino                                                               |
|                                                               SPIFlash library                                                                |
|                                                                   v 2.2.0                                                                     |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                    Marzogh                                                                    |
|                                                                  25.11.2015                                                                   |
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
#include<SPI.h>

/*
 * Uncomment the #define below if you would like real world readings.
 * For real world readings, hook up a light dependant resistor to A0.
 *
 */
//#define SENSOR

#ifdef SENSOR
const int LDR = A0;
#endif

const int cs = 10;


SPIFlash flash(cs);


struct Configuration {
  float lux;
  float vOut;                   // Voltage ouput from potential divider to Anolg input
  float RLDR;                   // Resistance calculation of potential divider with LDR
  bool light;
  uint8_t adc;
};
Configuration configuration;

void setup() {
  Serial.begin(115200);
  flash.begin();
  randomSeed(analogRead(A0));
  Serial.print(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println();
}

void loop() {
  uint16_t pageNo = random(0, 4095);
  uint8_t offset = random(0, 255);

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

  if (flash.writeAnything(pageNo, offset, configuration))
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
  //Serial.print("Number of bytes read back: ");
  flash.readAnything(pageNo, offset, configuration);
  flash.eraseSector(pageNo, 0);

  Serial.println("After reading");
  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);

  while (1) {}

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



