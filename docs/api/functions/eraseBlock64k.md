# eraseBlock64K (SPIFlash)

## Signatures

- `bool SPIFlash::eraseBlock64K(uint32_t _addr)`

## What It Is For

Erase the 64 KB block containing the given address.

## Parameters

- `_addr`: Any address within intended 64 KB erase window.

## Behavior Details

Best for large contiguous rewrites and reducing total erase command count.

## Return Semantics

Returns `true` on success.

## Failure Behavior

Unsupported opcode/chip profile mismatch or bus errors can fail this call.

## Common Mistakes

- Erasing a 64 KB block when only a small region needed reset.
- Failing to back up adjacent data in same block.
- Treating block erase boundaries as page boundaries.

## Example

```cpp
if (!flash.eraseBlock64K(addr)) {
  flash.error(VERBOSE);
}
```
