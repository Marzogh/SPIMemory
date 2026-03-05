# powerUp

## Signatures

- `bool SPIFlash::powerUp(void)`
- `bool SPIFram::powerUp(void)`

## What It Is For

Wake memory chip from low-power state so normal operations can resume.

## Parameters

- No parameters.

## Behavior Details

Call before any read/write after `powerDown` or deep sleep transitions.

## Return Semantics

Returns `true` when chip acknowledges wake-up sequence.

## Failure Behavior

If wake fails, subsequent operations may fail; check wiring/power timing and error code.

## Example

```cpp
if (!flash.powerUp()) {
  flash.error(VERBOSE);
}
```
