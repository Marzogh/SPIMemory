# begin

## Signatures

- `bool SPIFlash::begin(uint32_t flashChipSize = 0)`
- `bool SPIFram::begin(uint32_t flashChipSize = 0)`

## What It Is For

Initialize SPI memory communication, detect chip identity, load capacity/page behavior, and prepare the object for read/write operations.

## Parameters

- `flashChipSize`: Optional explicit size override for unsupported or custom chips. Leave `0` for automatic detection.

## Behavior Details

`begin()` must be called once in `setup()` before any data I/O. It configures communication, verifies chip support, and caches capacity/ID information used by every later call.

## Return Semantics

Returns `true` on successful initialization and chip identification. Returns `false` if comms/ID/support checks fail.

## Failure Behavior

Failure usually indicates wiring issues, wrong CS pin, unsupported chip, incorrect voltage levels, or SPI bus mismatch. Check `error(VERBOSE)` immediately.

## Common Mistakes

- Calling memory I/O before `begin()` has succeeded.
- Passing a custom `flashChipSize` that does not match real hardware.
- Ignoring a `false` return and continuing instead of checking `error(VERBOSE)`.

## Example

```cpp
SPIFlash flash(10);

void setup() {
  Serial.begin(115200);
  if (!flash.begin()) {
    flash.error(VERBOSE);
    while (1) {}
  }
}
```
