# powerDown

## Signatures

- `bool SPIFlash::powerDown(void)`
- `bool SPIFram::powerDown(void)`

## What It Is For

Put memory chip into low-power mode for energy-sensitive systems.

## Parameters

- No parameters.

## Behavior Details

Use in battery-driven designs between memory operations. Always pair with `powerUp()` before next read/write.

## Return Semantics

Returns `true` if command accepted by chip.

## Failure Behavior

Unsupported on some chips or states; subsequent read/write calls fail while powered down.

## Example

```cpp
flash.powerDown();
// sleep MCU
flash.powerUp();
```
