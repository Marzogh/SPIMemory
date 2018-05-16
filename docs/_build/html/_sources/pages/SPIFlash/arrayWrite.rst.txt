.. _arrayWrite:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

Array write functions
---------------------
.. cpp:function:: bool writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck = true)
* Writes an ``array of bytes`` to the address specified.

.. cpp:function:: bool writeCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool errorCheck = true)
* Writes an ``array of chars`` to the address specified.

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr

Address in memory where the data is to be written to. ``Mandatory``

.. cpp:var:: uint8_t *data_buffer
Pointer to data buffer to write the array from. ``Mandatory``

.. cpp:var:: size_t bufferSize
Size of the array to be written out to flash memory. ``Mandatory``

.. cpp:var:: bool errorCheck
Refer to :ref:`Advanced use <noteOnErrorCheck>` ``Optional``

What they do
~~~~~~~~~~~~~~
Write the array of values of the datatype and size specified by the user, to the address provided

Returns ``boolean``
~~~~~~~~~~~~~~~~~~~~~~
Returns ``TRUE`` if data written successfully, else returns ``FALSE``

Example code:
~~~~~~~~~~~~~~

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  #define _bufferSize 8

  uint8_t dataOut[_bufferSize] = {0,1,2,3,4,5,6,7};
  // This data type should be changed depending on the type of data being written to the flash memory
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(dataIn));
    Serial.print("Address = ");
    Serial.println(_address);

    Serial.print("Data write: ");
    if (flash.writeByteArray(_address, dataOut, _bufferSize) {
    // This function should be changed depending on the type of data being written to the flash memory
      Serial.println ("Successful");
    }
    else {
      Serial.println("Failed");
    }
    
  }

  void loop() {
  }
