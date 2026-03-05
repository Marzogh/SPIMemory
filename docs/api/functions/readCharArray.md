# readCharArray

## Signatures

- `bool SPIFlash::readCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool fastRead = false)`
- `bool SPIFram::readCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool fastRead = false)`

## What It Is For

Read a `char` span from memory into a caller-provided buffer.

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

## Common Mistakes

- Printing as C-string without ensuring null termination.
- Reading wrong byte count and truncating text.
- Using text APIs on binary data unintentionally.

## Example

```cpp
char rx[8] = {0};
if (flash.readCharArray(addr, rx, sizeof(rx))) {
  // parse rx
}
```
