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

## Common Mistakes

- Skipping `powerUp()` after deep sleep before first memory access.
- Assuming immediate readiness without checking first operation result.
- Ignoring wake failures and misattributing later read errors.

## Choosing Between Similar APIs

- Use `powerUp()` after explicit memory power-down or deep sleep.
- Skip only when chip was never placed in low-power state.
- If wake is flaky, reduce SPI speed and verify power sequencing.

## Example

```cpp
if (!flash.powerUp()) {
  flash.error(VERBOSE);
}
```
