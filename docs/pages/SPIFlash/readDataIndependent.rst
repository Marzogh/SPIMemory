.. _readDataIndependent:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

*******************************************************************
Data type-independent read function
*******************************************************************
.. cpp:function:: bool readAnything(uint32_t _addr, T& data, bool fastRead = false)

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr
Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: T& data
Variable to save the data to. ``Mandatory``

.. cpp:var:: bool fastRead
Refer to :ref:`Advanced use <noteOnFastRead>` ``Optional``

What it does
~~~~~~~~~~~~~~
Reads any type of variable / struct (any sized value) (starting) from a specific address and saves it to the variable ``T& data`` provided as an argument.

Returns ``boolean``
^^^^^^^^^^^^^^^^^^^^^
Returns ``TRUE`` if successful, ``FALSE`` if unsuccessful


.. note::
  This function can be used to replace any of the other read functions (except ``readByteArray()`` and ``readCharArray()``). However, if used for anything other than structs, this function runs slower than the data type-specific ones.

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
  } dataIn;
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(dataIn));
    Serial.print("Address = ");
    Serial.println(_address);
    Serial.print("readAnything()");
    if (!flash.readAnything(_address, dataIn)) { // Function is used to get the data from
                                               // address '_address' and save it to the struct 'test'
      Serial.println("Failed");
    }
    else {
      Serial.println("Passed");
    }
    ...
  }

  void loop() {
    ...
  }
