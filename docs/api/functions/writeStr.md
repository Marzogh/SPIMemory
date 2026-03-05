# writeStr

## Signatures

- `bool SPIFlash::writeStr(uint32_t _addr, String &data, bool errorCheck = true)`
- `bool SPIFram::writeStr(uint32_t _addr, String &data, bool errorCheck = true)`

## What It Is For

Write an Arduino `String` payload to memory in library-defined format.

## Parameters

- `_addr`: Start address for string storage.
- `data`: Input `String` to store.
- `errorCheck`: Optional verification pass after write.

## Behavior Details

For robust writes, reserve/erase enough bytes using `sizeofStr(data)` before writing. This avoids overlap/truncation when string lengths vary.

## Return Semantics

Returns `true` on successful write (and verification when enabled).

## Failure Behavior

Failures occur from insufficient erased space (flash), invalid address range, comms errors, or write-protect state.

## Example

```cpp
String msg = "Hello flash";
uint16_t sz = flash.sizeofStr(msg);
uint32_t addr = flash.getAddress(sz);
flash.eraseSection(addr, sz);
flash.writeStr(addr, msg);
```
