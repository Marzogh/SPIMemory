# functionRunTime

## Signatures

- `float SPIFlash::functionRunTime(void)`
- `float SPIFram::functionRunTime(void)`

## What It Is For

Return runtime metric for the most recent library operation, primarily for diagnostics and benchmarking.

## Parameters

- No parameters.

## Behavior Details

Use in test sketches to compare timing impact of clock speed, fast-read mode, and erase strategy.

## Return Semantics

Returns elapsed runtime value of previous operation.

## Failure Behavior

Interpret values carefully: runtime depends on operation type, chip state, and diagnostic compile settings.

## Example

```cpp
flash.writeByte(addr, 0x12);
Serial.print("Write runtime: ");
Serial.println(flash.functionRunTime());
```
