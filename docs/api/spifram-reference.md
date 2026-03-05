# SPIFram Function Reference

This page lists every public `SPIFram` call with beginner-focused usage notes.

## Constructors

### `SPIFram(uint8_t cs = CS)`
Default constructor for standard SPI bus.

### `SPIFram(uint8_t cs, SPIClass *spiinterface)`
Alternate SPI interface constructor on supported boards.

## Initialization and Chip Info

### `bool begin(uint32_t flashChipSize = 0)`
Initialize and identify FRAM device.

### `void setClock(uint32_t clockSpeed)`
Set SPI clock speed.

### `bool libver(uint8_t *b1, uint8_t *b2, uint8_t *b3)`
Read FRAM library version components.

### `uint8_t error(bool verbosity = false)`
Get last error code; optional verbose print.

### `uint16_t getManID(void)`
Get manufacturer ID.

### `uint32_t getJEDECID(void)`
Get JEDEC ID.

### `uint64_t getUniqueID(void)`
Get unique FRAM serial ID when supported.

### `uint32_t getAddress(uint16_t size)`
Find next free region for a block size.

### `uint16_t sizeofStr(String &inputStr)`
Compute String storage size helper.

### `uint32_t getCapacity(void)`
Get FRAM capacity in bytes.

### `float functionRunTime(void)`
Get runtime of previous operation.

## Single-Value IO

### `bool writeByte(uint32_t _addr, uint8_t data, bool errorCheck = true)`
### `uint8_t readByte(uint32_t _addr, bool fastRead = false)`

### `bool writeChar(uint32_t _addr, int8_t data, bool errorCheck = true)`
### `int8_t readChar(uint32_t _addr, bool fastRead = false)`

### `bool writeShort(uint32_t _addr, int16_t data, bool errorCheck = true)`
### `int16_t readShort(uint32_t _addr, bool fastRead = false)`

### `bool writeWord(uint32_t _addr, uint16_t data, bool errorCheck = true)`
### `uint16_t readWord(uint32_t _addr, bool fastRead = false)`

### `bool writeLong(uint32_t _addr, int32_t data, bool errorCheck = true)`
### `int32_t readLong(uint32_t _addr, bool fastRead = false)`

### `bool writeULong(uint32_t _addr, uint32_t data, bool errorCheck = true)`
### `uint32_t readULong(uint32_t _addr, bool fastRead = false)`

### `bool writeFloat(uint32_t _addr, float data, bool errorCheck = true)`
### `float readFloat(uint32_t _addr, bool fastRead = false)`

### `bool writeStr(uint32_t _addr, String &data, bool errorCheck = true)`
### `bool readStr(uint32_t _addr, String &data, bool fastRead = false)`

## Buffer IO

### `bool writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck = true)`
Write a byte buffer.

### `bool readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)`
Read a byte buffer.

### `bool writeCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool errorCheck = true)`
Write a char buffer.

### `bool readCharArray(uint32_t _addr, char *data_buffer, size_t buffer_size, bool fastRead = false)`
Read a char buffer.

## Generic IO

### `template <class T> bool writeAnything(uint32_t _addr, const T& data, bool errorCheck = true)`
Write custom type bytes.

### `template <class T> bool readAnything(uint32_t _addr, T& data, bool fastRead = false)`
Read custom type bytes.

## Clear/Erase

### `bool eraseSection(uint32_t _addr, uint32_t _sz)`
Clear a specific region.

### `bool eraseChip(void)`
Clear entire FRAM.

## Power

### `bool powerDown(void)`
Enter low-power mode.

### `bool powerUp(void)`
Exit low-power mode.

## Beginner Notes

- FRAM does not require flash-style sector erase before each write.
- Keep `errorCheck=true` during development.
- Validate behavior on your exact FRAM chip model.
