# readStr

## Signatures

- `bool SPIFlash::readStr(uint32_t _addr, String &data, bool fastRead = false)`
- `bool SPIFram::readStr(uint32_t _addr, String &data, bool fastRead = false)`

## What It Is For

Read a string previously written by `writeStr` from memory into a `String` object.

## Parameters

- `_addr`: Address where the string record begins.
- `data`: Output `String` passed by reference.
- `fastRead`: Optional faster read mode when stable on your platform.

## Behavior Details

Use matching address and write format from `writeStr`. Mixing manual binary layouts with `readStr` can produce invalid output.

## Return Semantics

Returns `true` when read/decode succeeded and output string was populated.

## Failure Behavior

False return indicates decode/read failure, invalid address span, or communication errors.

## Common Mistakes

- Reading from an address that was not written by `writeStr()`.
- Mixing manual binary layout with `readStr()` decoding.
- Ignoring `false` return and using stale output string.

## Choosing Between Similar APIs

- Use `readStr()` only for data written by `writeStr()`.
- Use `readCharArray()` for fixed buffer text decoding.
- Use `readByteArray()` for binary or mixed-format records.

## Example

```cpp
String out;
if (flash.readStr(addr, out)) {
  Serial.println(out);
} else {
  flash.error(VERBOSE);
}
```
