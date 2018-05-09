.. _Usage:

##############
API and Usage
##############

.. toctree::
   :maxdepth: 3
   :caption: Contents

   constructor.rst
   libInstantiate.rst

**************
Constructor:
**************
.. cpp:class:: SPIFlash


Library instantiation functions:
---------------------------------

  .. cpp:function:: bool begin(uint32_t flashChipSize = 0)
  .. cpp:function:: void setClock(uint32_t clockSpeed)
  .. cpp:function:: uint8_t readByte(uint32_t _addr, bool fastRead = false)
