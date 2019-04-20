.. _readArray:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

Array read functions
---------------------
.. cpp:function:: bool readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)

* Reads an ``array of bytes`` from the address specified, and saves the values to the ``data_buffer`` array provided.

.. cpp:function:: bool readCharArray(uint32_t _addr, char *data_buffer, size_t buffer_size, bool fastRead = false)

* Reads an ``array of chars`` from the address specified, and saves the values to the ``data_buffer`` array provided.

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr

Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: uint8_t *data_buffer

Pointer to data buffer to write the array to. ``Mandatory``

.. cpp:var:: size_t buffer_size

Size of the array to be read out from flash memory. ``Mandatory``

.. cpp:var:: bool fastRead

Refer to :ref:`Advanced use <noteOnFastRead>` ``Optional``

What they do
~~~~~~~~~~~~~~
Return the array of values of the datatype and size specified by the user, that is stored (starting) at the address provided

Returns ``boolean``
~~~~~~~~~~~~~~~~~~~~~~
Returns ``TRUE`` if data read successfully, else returns ``FALSE``

Example code:
~~~~~~~~~~~~~~

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  #define _bufferSize 8

  uint8_t dataIn[_bufferSize];
  // This data type should be changed depending on the type of data being read from the flash memory
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(dataIn));
    Serial.print(F("Address = "));
    Serial.println(_address);

    dataIn = flash.readByteArray(_address, dataIn, _bufferSize);
    // This function should be changed depending on the type of data being read from the flash memory

    Serial.print(F("Data read: "));
    for (uint8_t i = 0; i < _bufferSize; i++) {
      Serial.print(dataIn[i]);
      Serial.print(F(", "));
    }
    Serial.println();

  }

  void loop() {
  }
