# libver

## Signatures

- `bool SPIFlash::libver(uint8_t *b1, uint8_t *b2, uint8_t *b3)`
- `bool SPIFram::libver(uint8_t *b1, uint8_t *b2, uint8_t *b3)`

## What It Is For

Retrieve library semantic version components at runtime for debug logs and support reports.

## Parameters

- `b1`: Major version output pointer.
- `b2`: Minor version output pointer.
- `b3`: Patch version output pointer.

## Behavior Details

Writes version numbers through pointers so firmware can print exact library version in diagnostics.

## Return Semantics

Returns `true` if version values were written successfully.

## Failure Behavior

Passing null/invalid pointers leads to undefined behavior in C++ pointer semantics. Always pass addresses of `uint8_t` variables.

## Example

```cpp
uint8_t maj, min, pat;
if (flash.libver(&maj, &min, &pat)) {
  Serial.printf("SPIMemory %u.%u.%u\n", maj, min, pat);
}
```
