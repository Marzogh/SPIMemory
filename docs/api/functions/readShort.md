# readShort

## Signatures

- `int16_t SPIFlash::readShort(uint32_t _addr, bool fastRead = false)`
- `int16_t SPIFram::readShort(uint32_t _addr, bool fastRead = false)`

## What It Is For

Read one `int16_t` value from memory at a specific address.

## Parameters

- `_addr`: Address to read from.
- `fastRead`: Optional faster read path where supported; validate on your hardware before enabling broadly.

## Behavior Details

Reads raw bytes and decodes into `int16_t`. Pair this with matching write APIs to avoid type interpretation errors.

## Return Semantics

Returns decoded `int16_t` value from memory.

## Failure Behavior

If address is invalid or comms fail, returned value may be stale/default/invalid. Check `error()` after suspicious reads.

## Example

```cpp
int16_t out = flash.readShort(0);
Serial.println(out);
```
