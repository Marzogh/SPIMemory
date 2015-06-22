/* 
 *  Template
 *  #include <WProgram.h>
#define DEVICE 0x50 //this is the device ID from the datasheet of the 24LC256

//in the normal write anything the eeaddress is incrimented after the writing of each byte. The Wire library does this behind the scenes.

template <class T> int eeWrite(int ee, const T& value)
{
const byte* p = (const byte*)(const void*)&value;
int i;
Wire.beginTransmission(DEVICE);
Wire.send((int)(ee >> 8)); // MSB
Wire.send((int)(ee & 0xFF)); // LSB
for (i = 0; i < sizeof(value); i++)
Wire.send(*p++);
Wire.endTransmission();
return i;
}

template <class T> int eeRead(int ee, T& value)
{
byte* p = (byte*)(void*)&value;
int i;
Wire.beginTransmission(DEVICE);
Wire.send((int)(ee >> 8)); // MSB
Wire.send((int)(ee & 0xFF)); // LSB
Wire.endTransmission();
Wire.requestFrom(DEVICE,sizeof(value));
for (i = 0; i < sizeof(value); i++)
if(Wire.available())
*p++ = Wire.receive();
return i;
}
*
*
*/
//Code
/*
 * 
 * 
 #include <Wire.h>
#include "eepromi2c.h"

struct config
{
long targetLat;
long targetLon;
float fNum;
bool first;
int attempts;
} config;

void setup()
{

Serial.begin(9600);

//config some of the variables during to be saved. 
config.targetLat = 4957127;
config.targetLon = 6743421;
config.first = false;
config.attempts = 30;
config.fNum = 2.23;
eeWrite(0,config);
delay (30);
}

void loop()
{

//change the variables within the strucutre to show the read worked
config.targetLat =0;
config.targetLon = 0;
config.first = true;
config.fNum = 0.0;
config.attempts = 0;


eeRead(0,config);
delay(30);

Serial.print("lat = ");
Serial.println(config.targetLat);
Serial.print("lon =");
Serial.println(config.targetLon);

Serial.print("first");
Serial.println(config.first);

Serial.print("float");
Serial.println(config.fNum);

Serial.print("attempts = ");
Serial.println(config.attempts);

delay(1000);
}
 * 
 * 
 */

 /*
  * 
  * void writeDataArray(char type, uint8_t dataBuffer[])
{
  switch (type) {
    case 'b':
      writeBooleanArray();
      break;

    case 'B':
      writeByteArray();
      break;

    case 'I':
      writeIntArray();
      break;

    case 'L':
      writeLongArray();
      break;

    case 'F':
      writeFloatArray(2);
      break;
  }
}

void writeBooleanArray()
{
  int i;
  int j = 0;
  arraySize = arr_len(boolBuffer);
  while (j < 256) {
    for (i >= 0; i < arraySize; i++) {
      byte k, data = 0;
      while (k < 8) {
        bool value = boolBuffer[i];
        data = (data << 1) + value;
        i++;
        k++;
      }
      pageBuffer[j++] = data;
    }
  }
  flash.writePage(page, byteBuffer);
  page++;
}

  * void writeByteArray()
{
  flash.writePage(page, byteBuffer);
  page++;
}

void writeIntArray()
{
  int i;
  int j = 0;
  arraySize = arr_len(intBuffer);
  while (j < 256) {
    for (i >= 0; i < arraySize; i++) {
      int value = intBuffer[i];
      byte two = (value & 0xFF);
      byte one = ((value >> 8) & 0xFF);
      pageBuffer[j++] = one;
      pageBuffer[j++] = two;
    }
    flash.writePage(page, pageBuffer);
    j = 0;
    page++;
    resetPageBuffer();
  }
}

void writeLongArray()
{
  int i;
  int j = 0;
  arraySize = arr_len(longBuffer);
  while (j < 256) {
    for (i >= 0; i < arraySize; i++) {
      long value = longBuffer[i];
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);
      pageBuffer[j++] = one;
      pageBuffer[j++] = two;
      pageBuffer[j++] = three;
      pageBuffer[j++] = four;
    }
    flash.writePage(page, pageBuffer);
    j = 0;
    page++;
    resetPageBuffer();
  }
}

void writeFloatArray(uint8_t accuracy)                   //No of decimal points of accuracy
{
  int i;
  int j = 0;
  arraySize = arr_len(longBuffer);
  while (j < 256) {
    for (i >= 0; i < arraySize; i++) {
      long value = floatBuffer[i] * pow(10, accuracy);
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);
      pageBuffer[j++] = one;
      pageBuffer[j++] = two;
      pageBuffer[j++] = three;
      pageBuffer[j++] = four;
    }
    flash.writePage(page, pageBuffer);
    j = 0;
    page++;
    resetPageBuffer();
  }
}
  */
