# eraseSector (SPIFlash)

## Signatures

- `bool SPIFlash::eraseSector(uint32_t _addr)`

## What It Is For

Erase the 4 KB sector containing the provided address.

## Parameters

- `_addr`: Any address inside the target 4 KB sector.

## Behavior Details

Use this when data lives in one sector and you want minimal erase scope before rewriting.

## Return Semantics

Returns `true` on successful sector erase.

## Failure Behavior

Fails if chip is busy, write-enable path fails, address invalid, or erase opcode unsupported.

## Common Mistakes

- Expecting arbitrary byte-range erase; this targets 4 KB sector granularity.
- Erasing wrong sector by passing nearby but unintended address.
- Not checking return code before writing new data.

## Example

```cpp
uint32_t addr = 0x1200;
if (!flash.eraseSector(addr)) flash.error(VERBOSE);
```
