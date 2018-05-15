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
Data type-independent read functions
*******************************************************************

------------------------------------

readAnything()
---------------
.. cpp:function:: bool readAnything(uint32_t _addr, T& data, bool fastRead = false)

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr

    Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: T& data

    Variable to save the data to. ``Mandatory``

.. cpp:var:: bool fastRead

    Refer to :ref:`Note on fastRead <noteOnFastRead>` ``Optional``

What it does
~~~~~~~~~~~~~~
Reads any type of variable / struct (any sized value) (starting) from a specific address and saves it to the variable ``T& data`` provided as an argument.

Returns ``boolean``
^^^^^^^^^^^^^^^^^^^^^
Returns ``TRUE`` if successful, ``FALSE`` if unsuccessful


.. note::
  This function can be used to replace any of the read functions below (except ``readByteArray()`` and ``readCharArray()``). However, if used for anything other than structs, this function runs slower than the data type-specific ones below.

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

---------------------------------------

*******************************************************************
Data type-dependent read functions
*******************************************************************

------------------------------------

Single variable read
---------------------
.. cpp:function:: uint8_t readByte(uint32_t _addr, bool fastRead = false)
.. cpp:function:: bool readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)
.. cpp:function:: bool readCharArray(uint32_t _addr, char *data_buffer, size_t buffer_size, bool fastRead = false)
.. cpp:function:: int16_t readShort(uint32_t _addr, bool fastRead = false)
.. cpp:function:: uint16_t readWord(uint32_t _addr, bool fastRead = false)
.. cpp:function:: int32_t readLong(uint32_t _addr, bool fastRead = false)
.. cpp:function:: uint32_t readULong(uint32_t _addr, bool fastRead = false)
.. cpp:function:: float readFloat(uint32_t _addr, bool fastRead = false)
.. cpp:function:: bool readStr(uint32_t _addr, String &data, bool fastRead = false)

Parameters ``Mandatory & Optional``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr

    Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: bool fastRead

    Refer to :ref:`Note on fastRead <noteOnFastRead>` ``Optional``

What it does
~~~~~~~~~~~~~~
Returns ``uint8_t``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Reads an unsigned 8 bit integer (a ``byte``) from the address specified, and returns it.

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
    Serial.print("Address = ");
    Serial.println(_address);
    Serial.print("Data read : 0x");
    Serial.println(dataIn, HEX);
  }

  void loop() {
  }


------------------------------------

Array read
---------------------
.. cpp:function:: uint8_t readByte(uint32_t _addr, bool fastRead = false)
.. cpp:function:: bool readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)
.. cpp:function:: bool readCharArray(uint32_t _addr, char *data_buffer, size_t buffer_size, bool fastRead = false)
.. cpp:function:: int16_t readShort(uint32_t _addr, bool fastRead = false)
.. cpp:function:: uint16_t readWord(uint32_t _addr, bool fastRead = false)
.. cpp:function:: int32_t readLong(uint32_t _addr, bool fastRead = false)
.. cpp:function:: uint32_t readULong(uint32_t _addr, bool fastRead = false)
.. cpp:function:: float readFloat(uint32_t _addr, bool fastRead = false)
.. cpp:function:: bool readStr(uint32_t _addr, String &data, bool fastRead = false)

Parameters ``Mandatory & Optional``
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. cpp:var:: uint32_t _addr

    Address in memory where the data is to be read from. ``Mandatory``

.. cpp:var:: bool fastRead

    Refer to :ref:`Note on fastRead <noteOnFastRead>` ``Optional``

What it does
~~~~~~~~~~~~~~
Returns ``uint8_t``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Reads an unsigned 8 bit integer (a ``byte``) from the address specified, and returns it.

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
    Serial.print("Address = ");
    Serial.println(_address);
    Serial.print("Data read : 0x");
    Serial.println(dataIn, HEX);
  }

  void loop() {
  }

Related Errors ``CHIPISPOWEREDDOWN`` ``CALLBEGIN`` ``OUTOFBOUNDS`` ``CHIPBUSY``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* If the chip has previously been powered down and hasn't been powered up prior to calling this function, the library throws the error ``CHIPISPOWEREDDOWN``
* If ``begin()`` has not been called in ``void setup()``, the library throws the error ``CALLBEGIN``
* If the address to be read from is out of bounds - i.e. greater than the available memory on the chip - and :ref:`address overflow <addressOverflow>` has been disabled, the library throws the error ``OUTOFBOUNDS``
* If the chip is busy executing the a command passed to it previously or is locked up, the library throws the error ``CHIPBUSY``


Advanced use ``fastRead``
~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
