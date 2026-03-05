# writeCharArray

## Signatures

- `bool SPIFlash::writeCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool errorCheck = true)`
- `bool SPIFram::writeCharArray(uint32_t _addr, char *data_buffer, size_t bufferSize, bool errorCheck = true)`

## What It Is For

Write a `char` buffer for binary/text payloads with explicit length.

## Parameters

- `_addr`: Start address.
- `data_buffer`: Pointer to source bytes/chars.
- `bufferSize`: Number of bytes to write.
- `errorCheck`: Optional post-write verification.

## Behavior Details

For flash, erase destination region first. Ensure pointer remains valid for full call duration.

## Return Semantics

Returns `true` when all requested bytes were written successfully.

## Failure Behavior

Failures come from invalid pointers, address overflow, erase prerequisites (flash), or bus errors.

## Common Mistakes

- Assuming null terminator is auto-managed when `bufferSize` excludes it.
- Writing text payload with incorrect length.
- Skipping erase step on flash destinations.

## Example

```cpp
char payload[8] = {1,2,3,4,5,6,7,8};
flash.eraseSection(addr, sizeof(payload));
flash.writeCharArray(addr, payload, sizeof(payload));
```
