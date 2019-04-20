.. _writeDataIndependent:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

*******************************************************************
Data type-independent write function
*******************************************************************
.. cpp:function:: bool writeAnything(uint32_t _addr, const T& data, bool errorCheck = true)

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr
Address in memory where the data is to be written to. ``Mandatory``

.. cpp:var:: T& data
Variable to write. ``Mandatory``

.. cpp:var:: bool errorCheck
Refer to :ref:`Advanced use <noteOnErrorCheck>` ``Optional``

What it does
~~~~~~~~~~~~~~
Writes any type of variable / struct (any sized value) (starting) from a specific address (user provided)

Returns ``boolean``
^^^^^^^^^^^^^^^^^^^^^
Returns ``TRUE`` if successful, ``FALSE`` if unsuccessful


.. note::
  This function can be used to replace any of the other write functions (except ``writeByteArray()`` and ``writeCharArray()``). However, if used for anything other than structs, this function runs slower than the data type-specific ones.

Example code:
~~~~~~~~~~~~~~

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  struct testStruct {
    uint8_t _byte = 8;
    uint16_t _int = 269;
    uint32_t _long = 99432;
    float _float = 3.14;
    String _str = "This is a test string";
    uint8_t _array[8] = {0,1,2,3,4,5,6,7};
  } dataOut;
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(dataIn));
    Serial.print(F("Address = "));
    Serial.println(_address);

    Serial.print(F("writeAnything()"));
    if (!flash.writeAnything(_address, dataOut)) { // Function is used to write the data to
                                                   // address '_address'
      Serial.println(F("Failed"));
    }
    else {
      Serial.println(F("Passed"));
    }
    ...
  }

  void loop() {
    ...
  }
