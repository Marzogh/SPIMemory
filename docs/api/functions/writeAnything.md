# writeAnything

## Signatures

- `template <class T> bool SPIFlash::writeAnything(uint32_t _addr, const T& data, bool errorCheck = true)`
- `template <class T> bool SPIFram::writeAnything(uint32_t _addr, const T& data, bool errorCheck = true)`

## What It Is For

Write any trivially copyable object (struct, packed config, fixed-size type) as raw bytes.

## Parameters

- `_addr`: Start address.
- `data`: Object instance to serialize byte-for-byte.
- `errorCheck`: Enables optional verification after write.

## Behavior Details

Storage format is raw in-memory layout. Compiler packing/alignment/type definitions must remain consistent between write and read firmware versions.

## Return Semantics

Returns `true` on full write success.

## Failure Behavior

Changing struct layout between firmware versions breaks compatibility unless you version and migrate your data format.

## Example

```cpp
struct Config { uint16_t mode; float gain; } cfg{2, 1.25f};
flash.eraseSection(addr, sizeof(Config));
flash.writeAnything(addr, cfg);
```
