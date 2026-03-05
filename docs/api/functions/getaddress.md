# getAddress

## Signatures

- `uint32_t SPIFlash::getAddress(uint16_t size)`
- `uint32_t SPIFram::getAddress(uint16_t size)`

## What It Is For

Get next usable address region for writing a block of a given size without hand-maintaining offsets.

## Parameters

- `size`: Number of bytes you need reserved for the next object/buffer.

## Behavior Details

Helps avoid overlap bugs by automatically advancing internal allocation pointer.

## Return Semantics

Returns start address for the requested block.

## Failure Behavior

If allocation wraps or requested size exceeds available strategy assumptions, subsequent writes may fail address checks.

## Common Mistakes

- Assuming `getAddress()` persists across reboot without your own metadata scheme.
- Mixing manual offsets and allocator-style addresses accidentally.
- Not reserving exact sizes for variable-length payloads.

## Example

```cpp
uint32_t cfgAddr = flash.getAddress(sizeof(Config));
uint32_t nameAddr = flash.getAddress(flash.sizeofStr(name));
```
