# writeLong

## Signatures

- `bool SPIFlash::writeLong(uint32_t _addr, int32_t data, bool errorCheck = true)`
- `bool SPIFram::writeLong(uint32_t _addr, int32_t data, bool errorCheck = true)`

## What It Is For

Write one `int32_t` value to memory at a specific address.

## Parameters

- `_addr`: Target start address in memory.
- `data`: Value to persist.
- `errorCheck`: When `true`, read-back verification is performed after write for stronger integrity checking.

## Behavior Details

For flash, target region must be erased beforehand (typically 0xFF) for reliable programming. FRAM usually allows direct overwrite without explicit erase, but capacity checks still apply.

## Return Semantics

Returns `true` when write sequence completes (and optional verification passes). Returns `false` on address, power-state, bus, or verification errors.

## Failure Behavior

Common failures: out-of-range address, flash not erased, chip busy, powered-down device, or SPI signal integrity problems.

## Example

```cpp
uint32_t addr = 0;
// flash.eraseSector(addr); // required for flash if not erased
bool ok = flash.writeLong(addr, -10959);
if (!ok) flash.error(VERBOSE);
```
