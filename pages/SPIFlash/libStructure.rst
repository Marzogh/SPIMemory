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

  .. cpp:function:: bool begin(uint32_t flashChipSize = 0)
  .. cpp:function:: void setClock(uint32_t clockSpeed)

Chip ID functions:
---------------------------------

  .. cpp:function:: uint16_t getManID(void)
  .. cpp:function:: uint32_t getJEDECID(void)
  .. cpp:function:: uint64_t getUniqueID(void)
  .. cpp:function:: uint32_t getCapacity(void)
  .. cpp:function:: uint32_t getMaxPage(void)

Read functions:
---------------------------------
  ``Data type-independent``
  
  .. cpp:function:: bool readAnything(uint32_t _addr, T& data, bool fastRead = false)

  ``Data type-dependent``

  .. cpp:function:: uint8_t readByte(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: int8_t readChar(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: int16_t readShort(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: uint16_t readWord(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: int32_t readLong(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: uint32_t readULong(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: float readFloat(uint32_t _addr, bool fastRead = false)
  .. cpp:function:: bool readStr(uint32_t _addr, String &data, bool fastRead = false)
  .. cpp:function:: bool readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)
  .. cpp:function:: bool readCharArray(uint32_t _addr, char *data_buffer, size_t buffer_size, bool fastRead = false)
