# readAnything

## Signatures

- `template <class T> bool SPIFlash::readAnything(uint32_t _addr, T& data, bool fastRead = false)`
- `template <class T> bool SPIFram::readAnything(uint32_t _addr, T& data, bool fastRead = false)`

## What It Is For

Read raw bytes into an object of type `T`, reversing `writeAnything`.

## Parameters

- `_addr`: Address where object bytes begin.
- `data`: Output object reference to fill.
- `fastRead`: Optional read acceleration mode.

## Behavior Details

Only safe when `T` layout matches exactly what was written (same fields/order/types/packing).

## Return Semantics

Returns `true` when object bytes were read successfully.

## Failure Behavior

Mismatched type layout gives corrupted values even if API reports success.

## Example

```cpp
Config cfgOut{};
if (flash.readAnything(addr, cfgOut)) {
  Serial.println(cfgOut.gain);
}
```
