# readChar

## Signatures

- `int8_t SPIFlash::readChar(uint32_t _addr, bool fastRead = false)`
- `int8_t SPIFram::readChar(uint32_t _addr, bool fastRead = false)`

## What It Is For

Read one `int8_t` value from memory at a specific address.

## Parameters

- `_addr`: Address to read from.
- `fastRead`: Optional faster read path where supported; validate on your hardware before enabling broadly.

## Behavior Details

Reads raw bytes and decodes into `int8_t`. Pair this with matching write APIs to avoid type interpretation errors.

## Return Semantics

Returns decoded `int8_t` value from memory.

## Failure Behavior

If address is invalid or comms fail, returned value may be stale/default/invalid. Check `error()` after suspicious reads.

## Example

```cpp
int8_t out = flash.readChar(0);
Serial.println(out);
```
