.. _readSingleVariable:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

Single variable read functions
--------------------------------
These functions are designed to read a single variable - of a specific data type - from the flash memory. Each supported type of variable has a function specific to it - as listed below:

.. cpp:function:: uint8_t readByte(uint32_t _addr, bool fastRead = false)
* Reads an unsigned 8 bit integer (a ``byte``) from the address specified, and returns it.

.. cpp:function:: int8_t readChar(uint32_t _addr, bool fastRead = false)
* Reads a signed 8 bit integer (a ``char``) from the address specified, and returns it.

.. cpp:function:: int16_t readShort(uint32_t _addr, bool fastRead = false)
* Reads a signed 16 bit integer (a ``short``) from the address specified, and returns it.

.. cpp:function:: uint16_t readWord(uint32_t _addr, bool fastRead = false)
* Reads an unsigned 16 bit integer (a ``word``) from the address specified, and returns it.

.. cpp:function:: int32_t readLong(uint32_t _addr, bool fastRead = false)
* Reads a signed 32 bit integer (a ``long``) from the address specified, and returns it.

.. cpp:function:: uint32_t readULong(uint32_t _addr, bool fastRead = false)
* Reads an unsigned 32 bit integer (an ``unsigned long``) from the address specified, and returns it.

.. cpp:function:: float readFloat(uint32_t _addr, bool fastRead = false)
* Reads a ``float`` from the address specified, and returns it.

.. cpp:function:: bool readStr(uint32_t _addr, String &data, bool fastRead = false)
* Reads a ``String`` from the address specified, and returns it.

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr
Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: bool fastRead
Refer to :ref:`Advanced use <noteOnFastRead>` ``Optional``

What they do
~~~~~~~~~~~~~~
Return the value (of the datatype specified) that is stored at the address provided

Example code:
~~~~~~~~~~~~~~

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  uint8_t dataIn;     // This data type should be changed depending on the type of data
                        // being read from the flash memory
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(dataIn));
    dataIn = flash.readByte(_address);    // This function should be changed depending on the type of data
                                          // being read from the flash memory
    Serial.print(F("Address = "));
    Serial.println(_address);
    Serial.print(F("Data read : 0x"));
    Serial.println(dataIn, HEX);
  }

  void loop() {
  }
