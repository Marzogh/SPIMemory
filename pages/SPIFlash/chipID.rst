.. _ChipID:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

####################################################
Chip ID
####################################################

``SFDP compatibility dependent``

A number of functions are available to the library user to identify chip in use.

.. note::

  The library is designed to identify the flash memory chip in use when the ``begin();`` function is called (using the internal function ``_chipID();``). This happens in two ways:

  * If the flash memory supports the SFDP standard, then, the SFDP tables are used to identify the chip. The information from the tables is usually enough to let the library execute all functions without impediment, so any Chip ID errors thrown (if any) can be and, are ignored.

  * If the flash memory does not support the SFDP standard, three situations can arise:

    * If the chip is officially supported by the library, its can be identified by internal methods.
    * If the chip is not officially supported, then the user has to declare the size as an argument when calling the ``begin();`` function. This information is usually enough to let the library execute all functions without impediment, so any Chip ID errors thrown (if any) can be and, are ignored.
    * If the chip canot be ID'd and the user does not declare a size in ``begin();``, the library throws an error - ``UNKNOWNCHIP`` as soon as the function ``begin();`` is called. (Refer to  :ref:`Diagnostics & Error reporting <ErrorReporting>`)

--------------------------------------------------

*******************************************************************
getManID()
*******************************************************************
.. cpp:function:: uint16_t getManID(void)

Parameters ``None``
----------------------------

What it does
------------
Returns ``uint16_t``
~~~~~~~~~~~~~~~~~~~~~~
Returns the Manufacturer ID as a 16 bit unsigned integer


Example code:
---------------------

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    flash.begin();
    uint16_t manID = flash.getManID();      // Function is used to get the manufacturer ID and store it as
                                            // a 16 bit unsigned integer
    Serial.print(F("Chip Manufacturer ID: 0x"));
    Serial.println(manID, HEX);                  // The manufacturer ID is printed as a Hexadecimal number
    ...
  }

  void loop() {
    ...
  }

Related Errors ``None``
----------------------------

Advanced use ``N/A``
----------------------------

------------------------------------------

.. _getJEDECID():

*******************************************************************
getJEDECID()
*******************************************************************
.. cpp:function:: uint32_t getJEDECID(void)

Parameters ``None``
----------------------------

What it does
-------------
Returns ``uint32_t``
~~~~~~~~~~~~~~~~~~~~~~
Returns the JEDEC ID as a 32 bit unsigned integer


Example code:
---------------------

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    flash.begin();
    uint32_t JEDEC = flash.getJEDECID();      // Function is used to get the JEDEC ID and store it as
                                              // a 32 bit unsigned integer
    Serial.print(F("JEDEC ID: 0x"));
    Serial.println(JEDEC, HEX);                  // The JEDEC ID is printed as a Hexadecimal number
    ...
  }

  void loop() {
    ...
  }

Related Errors ``UNKNOWNCHIP``
-------------------------------
The way this function executes depends on whether the flash memory chip complies with the SFDP standard.

* If the chip supports SFDP (immaterial of official support), then, the library will work as it should - immaterial of whether or not it can read the JEDEC ID (even if it throws the error ``UNKNOWNCHIP``).
* If the chip is does not support SFDP and the chip is unable to read the JEDEC ID (internally in the ``begin();`` function), then it throws the error ``UNKNOWNCHIP``

Advanced use ``N/A``
----------------------------

------------------------------------------

*******************************************************************
getUniqueID()  ``Memory IC dependent``
*******************************************************************
.. cpp:function:: uint64_t getUniqueID(void)

Parameters ``None``
----------------------------

What it does
-------------
Returns ``uint64_t``
~~~~~~~~~~~~~~~~~~~~~~
Returns the flash memory chip's unique ID as a 64 bit unsigned integer


Example code:
---------------------

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    flash.begin();
    uint64_t uniqueID = flash.getUniqueID();      // Function is used to get the unique ID and store it as
                                                  // a 64 bit unsigned integer
    Serial.print(F("Unique ID: 0x"));
    Serial.println(uniqueID, HEX);                  // The unique ID is printed as a Hexadecimal number
    ...
  }

  void loop() {
    ...
  }

Related Errors ``None``
----------------------------

Advanced use ``N/A``
----------------------------

------------------------------------------

.. _getCapacity():

*******************************************************************
getCapacity()
*******************************************************************
.. cpp:function:: uint32_t getCapacity(void)

Parameters ``None``
----------------------------

What it does
-------------
Returns ``uint32_t``
~~~~~~~~~~~~~~~~~~~~~~
Returns the flash memory chip's capacity as a 32 bit unsigned integer


Example code:
---------------------

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    flash.begin();
    uint32_t cap = flash.getCapacity();      // Function is used to get the unique ID and store it as
                                                  // a 32 bit unsigned integer
    Serial.print(F("Capacity: "));
    Serial.println(cap);               // The unique ID is printed as a decimal number - in bytes
    ...
  }

  void loop() {
    ...
  }

Related Errors  ``UNKNOWNCAP``
-------------------------------
If the chip's capacity cannot be determined, the library throws an error - ``UNKNOWNCAP`` as soon as the function ``begin();`` is called. (Refer to  :ref:`Diagnostics & Error reporting <ErrorReporting>`)

Advanced use ``N/A``
----------------------------


.. note::

  The way this function executes depends on whether the flash memory chip complies with the SFDP standard.

  * The chip's capacity is determined in one of three ways:
      * If the chip supports SFDP (immaterial of official support), then, the chip's capacity is read from the SFDP tables.
      * If the chip is officially supported by the library, its capacity is already known.
      * If the chip is not officially supported, then the user has to declare the size as an argument when calling the ``begin()`` function.

------------------------------------------

*******************************************************************
getMaxPage()
*******************************************************************
.. cpp:function:: uint32_t getMaxPage(void)

Parameters ``None``
----------------------------

What it does
-------------
Returns ``uint32_t``
~~~~~~~~~~~~~~~~~~~~~~
Returns the number of physical `pages` in the flash memory as a 32 bit unsigned integer


Example code:
---------------------

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    flash.begin();
    uint32_t maxPage = flash.getMaxPage();  // Function is used to get the number of pages and store it as
                                            // a 32 bit unsigned integer
    Serial.print(F("Maximum pages: "));
    Serial.println(maxPage);                  // The number of pages is printed
    ...
  }

  void loop() {
    ...
  }

Related Errors ``None``
----------------------------

Advanced use ``N/A``
----------------------------

.. note::

  The way this function executes depends on whether the flash memory chip complies with the SFDP standard.

  * If the chip supports SFDP, then, the chip's capacity and page size (in bytes) are read from the SFDP tables.

  * If the chip does not support SFDP, then the chip's capacity is determined in one of two ways (refer to the note in :ref:`getCapacity()`). The pagesize uses the default (and most common) value of 256 bytes per page.

------------------------------------------
