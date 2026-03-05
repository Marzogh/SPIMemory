# resumeProg (SPIFlash)

## Signatures

- `bool SPIFlash::resumeProg(void)`

## What It Is For

Resume a previously suspended program/erase operation.

## Parameters

- No parameters.

## Behavior Details

Pair this with `suspendProg` to complete long operations after temporary interruption.

## Return Semantics

Returns `true` on successful resume.

## Failure Behavior

Fails if no suspend state exists, command unsupported, or chip state invalid.

## Common Mistakes

- Calling resume without a valid suspended operation.
- Assuming resume success without verifying final data integrity.
- Ignoring failed resume and continuing normal writes immediately.

## Choosing Between Similar APIs

- Use `resumeProg()` only after confirmed suspend state.
- If suspend is unsupported, keep single-operation flow without resume logic.
- Validate final data after resume on your exact chip.

## Example

```cpp
flash.suspendProg();
// do urgent task
flash.resumeProg();
```
