.. _advancedRead:

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4


.. _noteOnFastRead:

Advanced use ``fastRead``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
