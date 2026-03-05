# sfdpPresent (SPIFlash)

## Signatures

- `bool SPIFlash::sfdpPresent(void)`

## What It Is For

Check whether Serial Flash Discoverable Parameters (SFDP) are present and readable on the connected flash chip.

## Parameters

- No parameters.

## Behavior Details

SFDP presence can improve compatibility and timing/erase parameter discovery on supported parts.

## Return Semantics

Returns `true` if SFDP was detected/read, else `false`.

## Failure Behavior

`false` may be normal on older/non-SFDP chips. It is not automatically a fatal error if chip ID and operations still work.

## Example

```cpp
if (flash.sfdpPresent()) {
  Serial.println("SFDP available");
} else {
  Serial.println("SFDP not available");
}
```
