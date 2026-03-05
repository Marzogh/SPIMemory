# eraseSection

## Signatures

- `bool SPIFlash::eraseSection(uint32_t _addr, uint32_t _sz)`
- `bool SPIFram::eraseSection(uint32_t _addr, uint32_t _sz)`

## What It Is For

Clear a contiguous region starting at `_addr` spanning `_sz` bytes.

## Parameters

- `_addr`: Start address for region clear.
- `_sz`: Region size in bytes.

## Behavior Details

On flash, this resolves to sector/block erase operations and is essential before rewriting programmed bits. On FRAM, this is library-level clear behavior.

## Return Semantics

Returns `true` when clear operation completed successfully.

## Failure Behavior

Invalid ranges, protected states, busy chip state, or unsupported erase granularity can cause failure.

## Example

```cpp
uint32_t len = sizeof(Config);
flash.eraseSection(addr, len);
```
