# SPIFram API

For full per-call detail, use the [SPIFram Function Reference](spifram-reference.md).

## Status

FRAM support exists in the library and should be treated as a narrower compatibility surface than SPIFlash.

## Functional groups

- Initialization and identification
- Primitive and typed read/write
- Region/chip clear operations
- Power control

## Notes

- FRAM write semantics differ from flash (no erase-before-write requirement).
- Validate behavior on your exact FRAM part number.
