# writeChar

## Signatures

- `bool SPIFlash::writeChar(uint32_t _addr, int8_t data, bool errorCheck = true)`
- `bool SPIFram::writeChar(uint32_t _addr, int8_t data, bool errorCheck = true)`

## What It Is For

Write one `int8_t` value to memory at a specific address.

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

## Common Mistakes

- Writing to flash without erasing target region first.
- Ignoring `false` return and not checking `error(VERBOSE)`.
- Reusing addresses without a clear storage map.

## Choosing Between Similar APIs

- Use this typed write when your payload is exactly this type.
- Use `writeAnything()` for composite structs.
- Use array/string writes when storing variable-length data.

## Example

```cpp
uint32_t addr = 0;
// flash.eraseSector(addr); // required for flash if not erased
bool ok = flash.writeChar(addr, -10);
if (!ok) flash.error(VERBOSE);
```
