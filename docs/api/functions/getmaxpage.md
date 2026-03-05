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

## Example

```cpp
Serial.print("Max pages: ");
Serial.println(flash.getMaxPage());
```
