# FlashDiagnostics.ino

Sketch path: `examples/FlashDiagnostics/FlashDiagnostics.ino`

## Purpose

Run a broad chip-health and library-behavior sweep before application development.

## Exact Test Inventory

Identity and metadata (from `getID()`):

- `flash.libver(&_ver, &_subver, &_bugfix)` (when `LIBVER` is enabled)
- `flash.getJEDECID()`
- `flash.getCapacity()`
- `flash.getMaxPage()`
- `flash.getUniqueID()`

Power tests:

- `flash.powerDown()`
- `flash.powerUp()`

Erase tests:

- `flash.eraseChip()`
- `flash.eraseSection(_addr, KB(72))`
- `flash.eraseBlock64K(_addr)`
- `flash.eraseBlock32K(_addr)`
- `flash.eraseSector(_addr)` (4 KB)

Primitive write/read tests:

- `flash.writeByte(addr, 35)` + `flash.readByte(addr)`
- `flash.writeChar(addr, -110)` + `flash.readChar(addr)`
- `flash.writeWord(addr, 4520)` + `flash.readWord(addr)`
- `flash.writeShort(addr, -1250)` + `flash.readShort(addr)`
- `flash.writeULong(addr, 876532)` + `flash.readULong(addr)`
- `flash.writeLong(addr, -10959)` + `flash.readLong(addr)`
- `flash.writeFloat(addr, 3.14)` + `flash.readFloat(addr)`

Higher-level tests:

- `flash.writeStr(addr, "This is a test String 123!@#")` + `flash.readStr(addr, _data)`
- `flash.writeAnything(addr, testStruct)` + `flash.readAnything(addr, testStructOut)`
- `flash.writeByteArray(addr, _d, 256)` + `flash.readByteArray(addr, _data, 256)`

Timing and diagnostics:

- `flash.functionRunTime()` sampled per operation
- PASS/FAIL from readback/value comparisons

## Why Run It First

It gives the fastest full-surface confidence check. If this fails, application code is not the next thing to debug.

## Expected Output (Sample)

```text
Initialising..........

SPIMemory Library version: 3.x.x

JEDEC ID: 0xEF4017
Man ID: 0xEF
Memory ID: 0x40
Capacity: 8388608
Max Pages: 32768
Unique ID: ...

----------------------------------------------------------------
                        Testing library code
----------------------------------------------------------------
...
Byte        PASS
Char        PASS
Word        PASS
Short       PASS
ULong       PASS
Long        PASS
Float       PASS
Struct      PASS
Array       PASS
String      PASS
```

## Failure Signals

- `No comms. Check wiring. Is chip supported?`
- repeated `FAIL` rows in erase/data sections
- unstable pass/fail across repeated runs (often timing/wiring/noise)

## Common Root Causes

- wrong CS/SPI wiring or bus instance
- flash protection pin state (`WP`/`HOLD`) not valid
- no erase-before-write assumptions violated
- too-high SPI clock for wiring quality
