.. _writeSingleVariable:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

Single variable write functions
--------------------------------
These functions are designed to write a single variable - of a specific data type - to the flash memory. Each supported data type has a function specific to it - as listed below:

.. cpp:function:: bool writeByte(uint32_t _addr, uint8_t data, bool errorCheck = true)
* Writes an unsigned 8 bit integer - a ``byte``- to the address specified.

.. cpp:function:: bool writeChar(uint32_t _addr, int8_t data, bool errorCheck = true)
* Writes a signed 8 bit integer - a ``char``- to the address specified.

.. cpp:function:: bool writeShort(uint32_t _addr, int16_t data, bool errorCheck = true)
* Writes a signed 16 bit integer - a ``short``- to the address specified.

.. cpp:function:: bool writeWord(uint32_t _addr, uint16_t data, bool errorCheck = true)
* Writes an unsigned 16 bit integer - a ``word``- to the address specified.

.. cpp:function:: bool writeLong(uint32_t _addr, int32_t data, bool errorCheck = true)
* Writes a signed 32 bit integer - a ``long``- to the address specified.

.. cpp:function:: bool writeULong(uint32_t _addr, uint32_t data, bool errorCheck = true)
* Writes an unsigned 32 bit integer - an ``unsigned long``- to the address specified.

.. cpp:function:: bool writeFloat(uint32_t _addr, float data, bool errorCheck = true)
* Writes a ``float`` to the address specified.

.. cpp:function:: bool writeStr(uint32_t _addr, String &data, bool errorCheck = true)
* Writes a ``String`` to the address specified.

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr

Address in memory where the data is to be written to. ``Mandatory``

.. cpp:var:: size_t data
Data variable to write to the flash memory. ``Mandatory``

.. cpp:var:: bool errorCheck
Refer to :ref:`Advanced use <noteOnErrorCheck>` ``Optional``

What they do
~~~~~~~~~~~~~~
Write the data (of the datatype and size specified by the user) to the address provided

Returns ``boolean``
~~~~~~~~~~~~~~~~~~~~~~
Returns ``TRUE`` if data written successfully, else returns ``FALSE``

Example code:
~~~~~~~~~~~~~~

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  uint32_t _address;
  // This data type should be changed depending on the type of data being write to the flash memory
  String dataOut = "This is a test String!";


  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(dataIn));
    Serial.print("Address = ");
    Serial.println(_address);

    Serial.print("Data write ");
    // This function should be changed depending on the type of data being written to the flash memory
    if (flash.writeStr(_address, dataOut)) {
      Serial.println("successful");
    }
    else {
      Serial.println("failed");
    }
  }

  void loop() {
  }
