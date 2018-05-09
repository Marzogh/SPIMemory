.. _InstantiationFunc

#################################
Library Instantiation functions
#################################

These functions set up the library for use and should be called as required.

-----------------------------------------------------------------------------------------------------------

*******************************************************************
begin() ``Required for library operation``
*******************************************************************
.. cpp:function:: bool SPIFlash::begin(uint32_t flashChipSize = 0)
Parameters ``Optional``
----------------------------
  .. cpp:var:: uint32_t flashChipSize

    Refer to ``Advanced use`` below
What it does
-------------
* Must be called at the start in ``void setup()``. This function detects the type of chip being used and sets parameters accordingly.

* This function is essential to the functioning of the library and must be called before any other calls are made to the library.

Advanced use
-------------
* An optional ``flashChipSize`` parameter can be declared as an argument with this function (if library version > v2.6.0)

* In an instance where the library is being used with a flash memory chip that is not officially supported by the Library, declaring the chip storage size in *bits* as the ``flashChipSize`` parameter can - in many instances - enable the library to work with the chip

Related Errors
---------------
If this function is not called the library throws an error - CALLBEGIN. (Refer to  :ref:`Diagnostics & Error reporting <ErrorReporting>`)

-----------------------------------------------------------------------------------------------------------

*******************************************************************
setClock() ``Use with care``
*******************************************************************
.. cpp:function:: bool SPIFlash::setClock(uint32_t clockSpeed)

Parameters ``Non-optional``
----------------------------
  .. cpp:var:: uint32_t clockSpeed

    A 32 bit unsigned integer that represents SPI Clock Speed in Hertz

What it does
-------------
* This is an optional function and is used to set the SPI clock speed for all further comms using the library.

* If required, this function must be called straight after begin().

* This function takes a 32-bit value (in Hertz) as replacement for the default maximum clock speed (104MHz for Winbond NOR flash) thereby initiating future SPI transactions with the user-defined clock speed.

For example:

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    flash.begin();
    flash.setClock(20000000);     // this sets the clock spped to 20,000,000 Hz - i.e. 20MHz
    ...
  }

  void loop() {
    ...
  }
