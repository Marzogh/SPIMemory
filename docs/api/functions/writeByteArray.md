# writeByteArray

## Signatures

- `bool SPIFlash::writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck = true)`
- `bool SPIFram::writeByteArray(uint32_t _addr, uint8_t *data_buffer, size_t bufferSize, bool errorCheck = true)`

## What It Is For

Write a `uint8_t` buffer for binary/text payloads with explicit length.

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

- Passing buffer length that does not match real payload size.
- Forgetting erase-before-write on flash region.
- Using invalid/temporary buffer pointers.

## Example

```cpp
uint8_t payload[8] = {1,2,3,4,5,6,7,8};
flash.eraseSection(addr, sizeof(payload));
flash.writeByteArray(addr, payload, sizeof(payload));
```
