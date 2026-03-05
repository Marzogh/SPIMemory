# Example Sketches Guide (Troubleshooting + Learning)

This page is now the hub. Each bundled example has its own dedicated documentation page.

## Recommended Beginner Order

1. [FlashDiagnostics.ino](examples/flashdiagnostics.md)
2. [TestFlash.ino](examples/testflash.md)
3. [readWriteString.ino](examples/readwritestring.md)
4. [getAddressEx.ino](examples/getaddressex.md)
5. [Struct_writer.ino](examples/struct-writer.md)
6. [Diagnostics_functions.ino](examples/diagnostics-functions.md)

## Per-Sketch Pages

- [FlashDiagnostics.ino](examples/flashdiagnostics.md)
- [TestFlash.ino](examples/testflash.md)
- [getAddressEx.ino](examples/getaddressex.md)
- [readWriteString.ino](examples/readwritestring.md)
- [Struct_writer.ino](examples/struct-writer.md)
- [Diagnostics_functions.ino](examples/diagnostics-functions.md)

## What You Get On Each Page

- exact API call inventory used by the sketch
- expected serial output samples
- failure signals and common root causes
- practical debugging interpretation

### Helper functions defined in this file

- Output helpers: `printLine`, `printTab`, `printTime`, `printTimer`, `pass`
- Identity helpers: `printUniqueID`, `getID`
- Test routines:
  - `powerDownTest`, `powerUpTest`
  - `eraseChipTest`, `eraseSectionTest`, `eraseBlock64KTest`, `eraseBlock32KTest`, `eraseSectorTest`
  - `byteTest`, `charTest`, `wordTest`, `shortTest`, `uLongTest`, `longTest`, `floatTest`, `structTest`, `arrayTest`, `stringTest`

## Recommended order for beginners

1. `FlashDiagnostics.ino`
2. `TestFlash.ino` (targeted operation isolation)
3. `readWriteString.ino`
4. `getAddressEx.ino`
5. `Struct_writer.ino`

## What to capture when asking for help

- Board and core version
- Chip part number
- Wiring summary (including `WP/HOLD` if flash)
- Example sketch name
- Error code (`error()` value) and serial output snippet
