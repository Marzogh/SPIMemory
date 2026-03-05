# readFloat

## Signatures

- `float SPIFlash::readFloat(uint32_t _addr, bool fastRead = false)`
- `float SPIFram::readFloat(uint32_t _addr, bool fastRead = false)`

## What It Is For

Read one `float` value from memory at a specific address.

## Parameters

- `_addr`: Address to read from.
- `fastRead`: Optional faster read path where supported; validate on your hardware before enabling broadly.

## Behavior Details

Reads raw bytes and decodes into `float`. Pair this with matching write APIs to avoid type interpretation errors.

## Return Semantics

Returns decoded `float` value from memory.

## Failure Behavior

If address is invalid or comms fail, returned value may be stale/default/invalid. Check `error()` after suspicious reads.

## Common Mistakes

- Reading with wrong type/API compared to how data was written.
- Using out-of-range or stale addresses.
- Trusting suspicious values without checking error state.

## Choosing Between Similar APIs

- Use this typed read only for data written with matching type/layout.
- Use `readAnything()` for struct reconstruction.
- Use array/string reads for variable-length payloads.

## Example

```cpp
float out = flash.readFloat(0);
Serial.println(out);
```
