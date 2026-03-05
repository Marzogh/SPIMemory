# eraseChip

## Signatures

- `bool SPIFlash::eraseChip(void)`
- `bool SPIFram::eraseChip(void)`

## What It Is For

Clear entire memory device contents.

## Parameters

- No parameters.

## Behavior Details

Destructive operation used for factory reset, clean benchmark setup, or major schema reset.

## Return Semantics

Returns `true` when full erase/clear succeeds.

## Failure Behavior

Operation can be long on large flash chips. Power loss mid-operation may leave partially erased contents.

## Example

```cpp
if (confirmReset) {
  flash.eraseChip();
}
```
