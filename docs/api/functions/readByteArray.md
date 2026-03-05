# readByteArray

## Signatures

- `bool SPIFlash::readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)`
- `bool SPIFram::readByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool fastRead = false)`

## What It Is For

Read a `uint8_t` span from memory into a caller-provided buffer.

## Parameters

- `_addr`: Start address to read.
- `data_buffer`: Output buffer pointer.
- `bufferSize`: Number of bytes requested.
- `fastRead`: Optional performance mode.

## Behavior Details

Always size destination buffer to at least `bufferSize` bytes.

## Return Semantics

Returns `true` when requested bytes were read into buffer.

## Failure Behavior

If used as C-strings, ensure null termination is handled explicitly by your code.

## Example

```cpp
uint8_t rx[8] = {0};
if (flash.readByteArray(addr, rx, sizeof(rx))) {
  // parse rx
}
```
