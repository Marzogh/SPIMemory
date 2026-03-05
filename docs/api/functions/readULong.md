# readULong

## Signatures

- `uint32_t SPIFlash::readULong(uint32_t _addr, bool fastRead = false)`
- `uint32_t SPIFram::readULong(uint32_t _addr, bool fastRead = false)`

## What It Is For

Read one `uint32_t` value from memory at a specific address.

## Parameters

- `_addr`: Address to read from.
- `fastRead`: Optional faster read path where supported; validate on your hardware before enabling broadly.

## Behavior Details

Reads raw bytes and decodes into `uint32_t`. Pair this with matching write APIs to avoid type interpretation errors.

## Return Semantics

Returns decoded `uint32_t` value from memory.

## Failure Behavior

If address is invalid or comms fail, returned value may be stale/default/invalid. Check `error()` after suspicious reads.

## Common Mistakes

- Reading with wrong type/API compared to how data was written.
- Using out-of-range or stale addresses.
- Trusting suspicious values without checking error state.

## Example

```cpp
uint32_t out = flash.readULong(0);
Serial.println(out);
```
