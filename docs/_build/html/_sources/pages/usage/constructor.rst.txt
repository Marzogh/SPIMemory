.. _Constructor:

.. cpp:class:: SPIFlash

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

#############
Constructor
#############

********************
Definition
********************

  A constructor is a special kind of class member function that is executed when an object of that class is instantiated.
  Constructors are typically used to initialize member variables/functions of the class to appropriate default values, or to allow the user to easily initialize those member
  variables/functions to whatever values are desired. [*]_

********************
Usage
********************

* The constructor must be called before `void setup()`. The constructor can be any word of your choice. For example, the library can called by the following code where ``flash`` can be replaced by a constructor of the user's choice.

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash;

  void setup() {
    ...
  }

* The constructor is used to call a function from the SPIFlash library.

For example, in the code below, to call the
:cpp:func: `readByte()` function from the SPIFlash library, the line of code used is flash.readByte()

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

********************
Advanced Usage
********************

Defining a custom Chip Select pin
------------------------------------
The library can also called by declaring the `SPIFlash flash(csPin*)` constructor where 'flash' can be replaced by a user constructor of choice and `'csPin'` is the Chip Select pin for the flash module.
For example the following code sets up the library to use the `pin 33` as the Chip Select pin.

.. code-block:: cpp

  #include <SPIMemory.h>

  SPIFlash flash(33);

  void setup() {
  ...
  }

Using a non-default SPI interface [*]_
-----------------------------------

``IMPORTANT: the csPin must be declared if using a non-default SPI interface.``

All versions of the library >= v3.0.0 support the ability to use any of multiple SPI interfaces (if your µC supports them).

For example the following code sets up the library to use the SPI1 SPI interface instead of the default SPI0.

  #include SPIFlash.h

  SPIFlash flash(33, &SPI1);

  void setup() {
    ...
  }


.. rubric:: Footnotes
.. [*] `learncpp.com <http://www.learncpp.com/cpp-tutorial/85-constructors/>`_.
.. [*] This is currently only supported on the SAMD and STM32 architectures.
