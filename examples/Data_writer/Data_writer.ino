#include<SPIFlash.h>
#include<SPI.h>

#define SENSOR true

const int LDR = A0;
const int cs = 10;


#define pageLimit 4095
uint8_t pageBuffer[256];
/*boolean boolBuffer[2048];
uint8_t byteBuffer[256];
uint16_t intBuffer[128];
uint32_t longBuffer[64];
uint32_t floatBuffer[64];*/
uint32_t currentAddress;
//uint16_t page = 0;
//uint8_t offset, dataByte, arraySize;

//Lux stuff


SPIFlash flash(cs);


struct Configuration {
  float lux;
  float vOut;                   // voltage ouput from potential divider to Anolg input
  float RLDR;                   // Resistance calculation of potential divider with LDR
  bool light;
  uint8_t adc;
};
Configuration configuration;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initialising Flash memory"));
  for (int i = 0; i < 10; ++i)
  {
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println();
}

void loop() {
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
  //currentAddress = flash.writeAnything(0, 0, configuration);

  Serial.print("Saving amount: ");
  Serial.println(flash.writeAnything(2, 4, configuration));
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
  Serial.println("Local Set to 0");
  Serial.print("Reading back: ");
  Serial.println(flash.readAnything(2, 4, configuration));
  flash.eraseSector(2);
  
  Serial.println("After reading");
  Serial.println(configuration.lux);
  Serial.println(configuration.vOut);
  Serial.println(configuration.RLDR);
  Serial.println(configuration.light);
  Serial.println(configuration.adc);
  
  while(1){}
  
}

void readLDR()
{
  configuration.adc = analogRead(LDR);
  configuration.vOut = (configuration.adc * 0.0048828125);           // vOut = Output voltage from potential Divider. [vOut = ADC * (Vin / 1024)]
  configuration.RLDR = (10.0 * (5 - configuration.vOut)) / configuration.vOut;   // Equation to calculate Resistance of LDR, [R-LDR =(R1 (Vin - vOut))/ vOut]. R1 is in KOhms
  // R1 = 10 KOhms , Vin = 5.0 Vdc.
  configuration.lux = (500 / configuration.RLDR);
}

void resetPageBuffer()
{
  int s;
  for (s = 0; s < 256; s++) {
    pageBuffer[s] = 0;
  }
}

