.. _readFunc:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

###################################
Read functions
###################################

These functions enable the user to read data that is stored on the flash memory chip. The various functions listed return different data types as can be used as required.

------------------------------------

*******************************************************************
readAnything()
*******************************************************************
.. cpp:function:: bool readAnything(uint32_t _addr, T& data, bool fastRead = false);

Parameters ``Mandatory & Optional``
----------------------------
.. cpp:var:: uint32_t _addr

    Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: T& data

    Variable to save the data to. ``Mandatory``

.. cpp:var:: bool fastRead

    Refer to :ref:`Note on ``fastRead`` <noteOnFastRead>` ``Optional``

What it does
-------------
Returns ``Any valid data type``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Reads any type of variable / struct (any sized value) (starting) from a specific address and returns it.

.. note::
  This function can be used to replace any of the read functions below (except ``readByteArray()`` and ``readCharArray()``). However, if used for anything other than structs, this function runs slower than the data type-specific ones below.

Example code:
---------------------

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
  } test;
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(test));
    Serial.print("readAnything()");
    if (!flash.readAnything(_address, test)) { // Function is used to get the data from
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


Related Errors ``None``
----------------------------

Advanced use ``fastRead``
----------------------------
Refer to :ref:`Note on fastRead <noteOnFastRead>`

---------------------------------------

*******************************************************************
readAnything()
*******************************************************************
.. cpp:function:: bool readAnything(uint32_t _addr, T& data, bool fastRead = false);

Parameters ``Mandatory & Optional``
----------------------------
.. cpp:var:: uint32_t _addr

    Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: T& data

    Variable to save the data to. ``Mandatory``

.. cpp:var:: bool fastRead

    Refer to :ref:`Note on ``fastRead`` <noteOnFastRead>` ``Optional``

What it does
-------------
Returns ``Any valid data type``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Reads any type of variable / struct (any sized value) (starting) from a specific address and returns it.

.. note::
  This function can be used to replace any of the read functions below (except ``readByteArray()`` and ``readCharArray()``). However, if used for anything other than structs, this function runs slower than the data type-specific ones below.

Example code:
---------------------

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
  } test;
  uint32_t _address;

  void setup() {
    flash.begin();
    _address = flash.getAddress(sizeof(test));
    Serial.print("readAnything()");
    if (!flash.readAnything(_address, test)) { // Function is used to get the data from
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


Related Errors ``None``
----------------------------

Advanced use ``fastRead``
----------------------------
Refer to :ref:`Note on fastRead <noteOnFastRead>`

---------------------------------------

.. _noteOnFastRead:

.. note::

  **Note on** ``fastRead``

  * All read commands take a last boolean argument ``fastRead``. This argument defaults to ``FALSE``, and does not need to be specified when calling a function.

  For example:

  .. code-block:: cpp

    ...

    //Calling
    flash.readByte(addressToReadFrom);
    //or
    flash.readByte(addressToReadFrom, FALSE);
    //yields the same results.

    ...

  * However, when this argument is set to TRUE, it carries out the Fast Read instruction so data can be read at up to the memory's maximum frequency.

  .. code-block:: cpp

    ...

    //Calling
    flash.readByteArray(addressToReadFrom, bufferToReadTo, bufferSize, TRUE);
    //instead of
    flash.readByteArray(addressToReadFrom, bufferToReadTo, bufferSize);
    // will result in faster read speeds for very large data arrays.

    ...

  **This is useful only when reading very large amounts of data from the flash memory. If used for small arrays or individual variables, it will slow down the read function.**

---------------------------------------
