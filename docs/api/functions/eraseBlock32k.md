# eraseBlock32K (SPIFlash)

## Signatures

- `bool SPIFlash::eraseBlock32K(uint32_t _addr)`

## What It Is For

Erase the 32 KB block containing the provided address.

## Parameters

- `_addr`: Any address inside target 32 KB block.

## Behavior Details

Use for bulk updates where sector-by-sector erase would be slower.

## Return Semantics

Returns `true` when erase completes.

## Failure Behavior

May fail on chips lacking 32 KB erase opcode or when write/ready checks fail.

## Example

```cpp
if (!flash.eraseBlock32K(addr)) {
  flash.error(VERBOSE);
}
```
