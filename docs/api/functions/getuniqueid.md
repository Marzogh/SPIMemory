# getUniqueID

## Signatures

- `uint64_t SPIFlash::getUniqueID(void)`
- `uint64_t SPIFram::getUniqueID(void)`

## What It Is For

Read chip unique identifier when supported by the specific memory device.

## Parameters

- No parameters.

## Behavior Details

Useful for device fingerprinting, provisioning, and differentiating units in production logs.

## Return Semantics

Returns unique ID value, often `0` when unsupported/unavailable.

## Failure Behavior

Not all chips implement unique-ID commands; a zero value may reflect unsupported feature rather than total comms failure.

## Common Mistakes

- Assuming all chips implement unique ID and treating `0` as guaranteed comm failure.
- Using UID as security/authentication secret without threat modeling.
- Changing provisioning logic without handling unsupported chips.

## Example

```cpp
uint64_t uid = flash.getUniqueID();
Serial.println((uint32_t)(uid >> 32), HEX);
Serial.println((uint32_t)uid, HEX);
```
