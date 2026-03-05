# getManID

## Signatures

- `uint16_t SPIFlash::getManID(void)`
- `uint16_t SPIFram::getManID(void)`

## What It Is For

Read manufacturer identifier to confirm vendor family and validate bus communication.

## Parameters

- No parameters.

## Behavior Details

Useful during bring-up to verify chip is physically responding and not all-zero/all-ones bus noise.

## Return Semantics

Returns manufacturer ID value.

## Failure Behavior

Unexpected values usually indicate wiring/SPI issues or unsupported chips.

## Example

```cpp
Serial.print("Man ID: 0x");
Serial.println(flash.getManID(), HEX);
```
