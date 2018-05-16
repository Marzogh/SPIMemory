.. _libStructure:

.. toctree::
   :hidden:
   :maxdepth: 3
   :caption: Contents

.. default-domain:: cpp
.. highlight:: cpp
  :linenothreshold: 4

###################
Library structure
###################

**************
Constructor:
**************
.. cpp:class:: SPIFlash


Library instantiation functions:
---------------------------------

  .. cpp:function:: bool SPIFlash::begin(uint32_t flashChipSize = 0)
  .. cpp:function:: void SPIFlash::setClock(uint32_t clockSpeed)
  .. cpp:function:: uint8_t SPIFlash::readByte(uint32_t _addr, bool fastRead = false)
