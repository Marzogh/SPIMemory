# getMaxPage (SPIFlash)

## Signatures

- `uint32_t SPIFlash::getMaxPage(void)`

## What It Is For

Return maximum page index/count based on detected flash capacity and configured page size.

## Parameters

- No parameters.

## Behavior Details

Useful for page-based diagnostics and page iteration logic.

## Return Semantics

Returns max page value used by library page calculations.

## Failure Behavior

Using stale/hardcoded page limits can cause out-of-range accesses.

## Common Mistakes

- Confusing page count with byte capacity.
- Using page math from another chip without recalculating.
- Assuming page operations are equivalent to erase boundaries.

## Choosing Between Similar APIs

- Use `getMaxPage()` for page-oriented diagnostics/tooling.
- Use raw byte addresses for most application storage logic.
- Do not substitute page count for erase-block planning.

## Example

```cpp
Serial.print("Max pages: ");
Serial.println(flash.getMaxPage());
```
