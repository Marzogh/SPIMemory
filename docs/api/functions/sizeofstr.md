# sizeofStr

## Signatures

- `uint16_t SPIFlash::sizeofStr(String &inputStr)`
- `uint16_t SPIFram::sizeofStr(String &inputStr)`

## What It Is For

Compute storage footprint required by library string routines so allocations and erase sizes match serialized length.

## Parameters

- `inputStr`: String value to size for storage/readback routines.

## Behavior Details

Use with `getAddress()` and erase calls before writing strings to prevent under-allocation.

## Return Semantics

Returns required byte count for storing that specific `String`.

## Failure Behavior

If you erase/write fewer bytes than required, reads can truncate or include stale residual bytes.

## Example

```cpp
String name = "sensor-A";
uint16_t sz = flash.sizeofStr(name);
uint32_t addr = flash.getAddress(sz);
flash.eraseSection(addr, sz);
flash.writeStr(addr, name);
```
