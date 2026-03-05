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

## Common Mistakes

- Running chip erase in normal flow instead of explicit maintenance/reset paths.
- Not warning users that operation is destructive and long-running.
- Power-cycling mid-erase and assuming data integrity afterwards.

## Choosing Between Similar APIs

- Use `eraseChip()` for full reset/factory-clear flows.
- Use section/sector/block erase for normal incremental updates.
- Avoid chip erase in frequently called runtime paths.

## Example

```cpp
if (confirmReset) {
  flash.eraseChip();
}
```
