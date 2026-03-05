.. _readErrors:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

Related Errors ``CHIPISPOWEREDDOWN`` ``CALLBEGIN`` ``OUTOFBOUNDS`` ``CHIPBUSY``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* If the chip has previously been powered down and hasn't been powered up prior to calling this function, the library throws the error ``CHIPISPOWEREDDOWN``
* If ``begin()`` has not been called in ``void setup()``, the library throws the error ``CALLBEGIN``
* If the address to be read from is out of bounds - i.e. greater than the available memory on the chip - and :ref:`address overflow <addressOverflow>` has been disabled, the library throws the error ``OUTOFBOUNDS``
* If the chip is busy executing the a command passed to it previously or is locked up, the library throws the error ``CHIPBUSY``
