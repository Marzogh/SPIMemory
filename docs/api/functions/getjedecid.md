# getJEDECID

## Signatures

- `uint32_t SPIFlash::getJEDECID(void)`
- `uint32_t SPIFram::getJEDECID(void)`

## What It Is For

Read JEDEC triplet (manufacturer + memory type + capacity code) used to identify the chip model family.

## Parameters

- No parameters.

## Behavior Details

Primary identity check for memory chips. Many diagnostics and support flows begin with this value.

## Return Semantics

Returns JEDEC ID packed into a 32-bit value.

## Failure Behavior

Zero or unstable values indicate communication faults, wrong voltage, wrong chip-select, or unsupported devices.

## Example

```cpp
uint32_t jedec = flash.getJEDECID();
Serial.print("JEDEC: 0x");
Serial.println(jedec, HEX);
```
