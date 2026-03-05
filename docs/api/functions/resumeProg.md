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

## Example

```cpp
flash.suspendProg();
// do urgent task
flash.resumeProg();
```
