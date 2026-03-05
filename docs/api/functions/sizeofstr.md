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

## Common Mistakes

- Using `String.length()` instead of `sizeofStr()` for storage planning.
- Erasing fewer bytes than string storage actually requires.
- Reusing old allocation size after string content grows.

## Choosing Between Similar APIs

- Use `sizeofStr()` before `writeStr()` for exact storage planning.
- Use `writeCharArray()` when you need explicit fixed-length text layout.
- Prefer fixed-length arrays over `String` in hard real-time memory maps.

## Example

```cpp
String name = "sensor-A";
uint16_t sz = flash.sizeofStr(name);
uint32_t addr = flash.getAddress(sz);
flash.eraseSection(addr, sz);
flash.writeStr(addr, name);
```
