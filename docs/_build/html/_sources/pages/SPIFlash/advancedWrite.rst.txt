.. _advancedWrite:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4


.. _noteOnErrorCheck:

Advanced use ``errorCheck`` ``HIGHSPEED``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-----------

errorCheck ``WARNING: Data corruption likely``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* All write functions have Error checking turned on by default - i.e. every byte written to the flash memory will be checked against the data provided by the user.
* This is controlled by an optional boolean argument ``errorCheck``. This argument defaults to ``TRUE``, and does not need to be specified when calling a function.


For example:

.. code-block:: cpp

  ...

  //Calling
  flash.writeByte(addressToWriteTo, byteOfData);
  //or
  flash.writeByte(addressToWriteTo, byteOfData, FALSE);
  //yields the same results.

  ...

* Users who require greater write speeds can disable this function by setting the ``errorCheck`` argument in any write function to ``NOERRCHK``

For example:

.. code-block:: cpp

  ...

  //Calling
  flash.writeByte(addressToWriteTo, byteOfData, NOERRCHK);
  //instead of
  flash.writeByte(addressToWriteTo, byteOfData);
  // will result in faster write speeds at the risk of not catching any errors in the writing process.

  ...

**This is useful only when the program calls for increased writing speed over data integrity. Data writes cannot be guaranteed free of errors if errorCheck is turned off**

-----------

.. _noteOnHighSpeed:

Highspeed mode ``WARNING: Data corruption likely``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The library - by default - checks to see if the address being written to contains pre-existing data. If it finds pre-existing data, it throws the error ``PREVWRITTEN``. This prevents the write function from running as fast as it theoretically could.

If the user requires the library to operate at the highest speed possible, they must be sure that the user code does the following before every data write:

  * Makes sure that the flash memory chip does not contain any data before using it.

  * Erases a sector before writing to it.

  * Keeps track of addressing and makes sure no two bytes of data are ever written to the same address.

  * Tracks previously written addresses in the user code and makes sure to erase them before writing new data at those addresses.

Then, if ``#define HIGHSPEED`` is uncommented in `SPIFlash.h <https://github.com/Marzogh/SPIFlash/blob/762a8802f94e8f55713e56ce073aedd8a10bfe8d/src/SPIFlash.h#L45>`_ before the user code is compiled and uploaded, the write functions will run as fast as the flash memory hardware will permit them to.

.. note::
  ``**WARNING**``

  Please note that using a combination of the methods listed in :ref:`High speed mode <noteOnHighSpeed>` and :ref:`Error checking <noteOnErrorCheck>` will result in the highest possible write speed from the library. However, this will result in the highest probability of write errors / data corruption as well.

  ``**WARNING**``
