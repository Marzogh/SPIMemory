# error

## Signatures

- `uint8_t SPIFlash::error(bool verbosity = false)`
- `uint8_t SPIFram::error(bool verbosity = false)`

## What It Is For

Read the last library error code. Use verbose mode to print human-readable diagnostics over Serial.

## Parameters

- `verbosity`: Set `true` (or `VERBOSE`) to print error details in addition to returning code.

## Behavior Details

Call this right after a failed operation to capture meaningful context. Error state is tied to the most recent failing operation.

## Return Semantics

Returns numeric error code (`0` generally means no current error).

## Failure Behavior

Calling much later can hide root cause because newer operations may overwrite error context.

## Example

```cpp
if (!flash.writeByte(addr, 0x2A)) {
  uint8_t code = flash.error(VERBOSE);
  Serial.print("Error code: 0x");
  Serial.println(code, HEX);
}
```
