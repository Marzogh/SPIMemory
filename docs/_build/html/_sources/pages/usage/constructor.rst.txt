.. _Constructor:

.. cpp:class:: SPIFlash
.. cpp:function:: SPIFlash::SPIFlash(uint8_t cs = CS, SPIClass *spiinterface = &SPI)

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

#############
Constructor
#############

.. note::

  A constructor is a special kind of class member function that is executed when an object of that class is instantiated.
  Constructors are typically used to initialize member variables/functions of the class to appropriate default values, or to allow the user to easily initialize those member
  variables/functions to whatever values are desired. [*]_

************************
Parameters ``Optional``
************************
.. cpp:var:: uint8_t cs

    Refer to :ref:`Defining a custom Chip Select pin <customCSPin>`

.. cpp:var:: SPIClass *spiinterface

    Refer to :ref:`Using a non-default SPI interface <nonDefaultSPI>`

********************
What it does
********************

* The constructor must be called before ``void setup()``. The constructor can be any word of your choice. For example, the library can called by the example code below where ``flash`` can be replaced by a constructor of the user's choice.

* The constructor is used to call a function from the SPIFlash library.

********************
Example code:
********************

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;   //This is the constructor. This example uses 'flash' as the constructor

    void setup() {
    ...
    }

    void loop() {
    flash.readByte(...);  //The constructor 'flash' is used to call the function 'readByte()' from the library
    ...
    }

************************
Related Errors ``None``
************************

.. _advancedUseConstructor:

********************
Advanced Use
********************

.. _customCSPin:

Defining a custom Chip Select pin
------------------------------------
The library can also called by declaring the ``cs`` parameter in the constructor where ``cs`` is the uuser defined Chip Select pin for the flash module.

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash(33);   // The library uses the `pin 33` as the Chip Select pin instead of the default

  void setup() {
    ...
  }

  void loop {
    ...
  }

-------------------------------------

.. _nonDefaultSPI:

Using a non-default SPI interface [*]_
----------------------------------------

* The library currently only supports using non-default SPI interfaces on the following architectures:
    * SAMD
    * STM32
* The csPin **MUST** be declared if using a non-default SPI interface.
* Only available if library > v3.0.0

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash(33, &SPI1);  // The library now uses the 'SPI1' interface instead of the default 'SPI0'.
                              // It also uses pin 33 instead of the default Chip Select pin
  void setup() {
    ...
  }

  void loop {
    ...
  }

.. rubric:: Footnotes
.. [*] `learncpp.com <http://www.learncpp.com/cpp-tutorial/85-constructors/>`_.
.. [*] This is currently only supported on the SAMD and STM32 architectures.
