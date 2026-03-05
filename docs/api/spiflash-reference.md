# SPIFlash Function Reference

This page lists every public `SPIFlash` call with beginner-focused usage notes.

## Constructors

### `SPIFlash(uint8_t cs = CS)`
Use this on most boards with default SPI pins and a chosen chip-select pin.

### `SPIFlash(uint8_t cs, SPIClass *spiinterface)`
Use this on boards with multiple SPI buses (for example `SPI1`, `SPI2`).

### `SPIFlash(int8_t *SPIPinsArray)`
ESP32-style custom SPI pins constructor (`sck`, `miso`, `mosi`, `ss`).

## Initialization and Chip Info

### `bool begin(uint32_t flashChipSize = 0)`
Initialize SPI and identify the chip. Call once in `setup()`.

### `void setClock(uint32_t clockSpeed)`
Set SPI bus speed (transaction-capable cores).

### `bool libver(uint8_t *b1, uint8_t *b2, uint8_t *b3)`
Read library semantic version components.

### `bool sfdpPresent(void)`
Returns whether SFDP information is available.

### `uint8_t error(bool verbosity = false)`
Returns last library error code. Use `verbosity=true` to print details.

### `uint16_t getManID(void)`
Get manufacturer ID.

### `uint32_t getJEDECID(void)`
Get JEDEC ID (manufacturer + type + capacity ID bytes).

### `uint64_t getUniqueID(void)`
Read unique chip ID when supported.

### `uint32_t getCapacity(void)`
Returns chip capacity in bytes.

### `uint32_t getMaxPage(void)`
Returns max page count using internal page-size configuration.

### `float functionRunTime(void)`
Returns runtime for the previous library call (diagnostic mode use).

## Address Helpers

### `uint32_t getAddress(uint16_t size)`
Find next free region for a block of `size` bytes.

### `uint16_t sizeofStr(String &inputStr)`
Return the library storage size needed for a `String`.

## Single-Value IO

### `bool writeByte(uint32_t _addr, uint8_t data, bool errorCheck = true)`
Write one byte.

### `uint8_t readByte(uint32_t _addr, bool fastRead = false)`
Read one byte.

### `bool writeChar(uint32_t _addr, int8_t data, bool errorCheck = true)`
Write one signed 8-bit value.

### `int8_t readChar(uint32_t _addr, bool fastRead = false)`
Read one signed 8-bit value.

### `bool writeShort(uint32_t _addr, int16_t data, bool errorCheck = true)`
Write one signed 16-bit value.

### `int16_t readShort(uint32_t _addr, bool fastRead = false)`
Read one signed 16-bit value.

### `bool writeWord(uint32_t _addr, uint16_t data, bool errorCheck = true)`
Write one unsigned 16-bit value.

### `uint16_t readWord(uint32_t _addr, bool fastRead = false)`
Read one unsigned 16-bit value.

### `bool writeLong(uint32_t _addr, int32_t data, bool errorCheck = true)`
Write one signed 32-bit value.

### `int32_t readLong(uint32_t _addr, bool fastRead = false)`
Read one signed 32-bit value.

### `bool writeULong(uint32_t _addr, uint32_t data, bool errorCheck = true)`
Write one unsigned 32-bit value.

### `uint32_t readULong(uint32_t _addr, bool fastRead = false)`
Read one unsigned 32-bit value.

### `bool writeFloat(uint32_t _addr, float data, bool errorCheck = true)`
Write one `float`.

### `float readFloat(uint32_t _addr, bool fastRead = false)`
Read one `float`.

### `bool writeStr(uint32_t _addr, String &data, bool errorCheck = true)`
Write one `String`.

### `bool readStr(uint32_t _addr, String &data, bool fastRead = false)`
Read one `String`.

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
Write struct/custom type bytes as stored in memory.

### `template <class T> bool readAnything(uint32_t _addr, T& data, bool fastRead = false)`
Read struct/custom type bytes into an object.

## Erase Operations

### `bool eraseSection(uint32_t _addr, uint32_t _sz)`
Erase enough sectors/blocks for a region size.

### `bool eraseSector(uint32_t _addr)`
Erase one 4KB sector containing address.

### `bool eraseBlock32K(uint32_t _addr)`
Erase one 32KB block containing address.

### `bool eraseBlock64K(uint32_t _addr)`
Erase one 64KB block containing address.

### `bool eraseChip(void)`
Erase the entire flash chip.

## Power and Program State

### `bool suspendProg(void)`
Suspend eligible erase/program operations.

### `bool resumeProg(void)`
Resume a suspended operation.

### `bool powerDown(void)`
Put chip in low-power mode.

### `bool powerUp(void)`
Wake chip from power-down mode.

## Beginner Notes

- Flash usually requires erase-before-write.
- Start with default `errorCheck=true`.
- Enable `RUNDIAGNOSTIC` during bring-up.
