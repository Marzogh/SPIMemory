# Example Sketches Guide (Troubleshooting + Learning)

The bundled examples are one of the best parts of this library. This page explains what each one does, what success looks like, and what failures mean.

## `FlashDiagnostics/FlashDiagnostics.ino`

### Exact test inventory (every operation it executes)

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

Primitive data write/read tests:

- `flash.writeByte(addr, 35)` + `flash.readByte(addr)`
- `flash.writeChar(addr, -110)` + `flash.readChar(addr)`
- `flash.writeWord(addr, 4520)` + `flash.readWord(addr)`
- `flash.writeShort(addr, -1250)` + `flash.readShort(addr)`
- `flash.writeULong(addr, 876532)` + `flash.readULong(addr)`
- `flash.writeLong(addr, -10959)` + `flash.readLong(addr)`
- `flash.writeFloat(addr, 3.14)` + `flash.readFloat(addr)`

Higher-level data tests:

- `flash.writeStr(addr, "This is a test String 123!@#")` + `flash.readStr(addr, _data)`
- `flash.writeAnything(addr, testStruct)` + `flash.readAnything(addr, testStructOut)`
- `flash.writeByteArray(addr, _d, 256)` + `flash.readByteArray(addr, _data, 256)`

Timing and diagnostics:

- `flash.functionRunTime()` is sampled after each operation to print runtime columns.
- PASS/FAIL is decided by value comparison in each test function.

### Why run it first

It gives broad signal quickly: if this sketch fails, fix hardware/config before writing app logic.

### Expected success signs

- JEDEC ID prints non-zero.
- Function tests report pass.
- Read values match written values.

### Expected serial output (sample)

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

### Warning output to watch for

```text
No comms. Check wiring. Is chip supported?
```

or repeated `FAIL` rows in erase/data sections.

### Common failure patterns

- Fails immediately: wiring/CS/board SPI mismatch.
- Write tests fail: region not erased, `WP/HOLD` pin state, unstable power.
- Mixed intermittent failures: clock too fast, shared SPI bus interference.

---

## `TestFlash/TestFlash.ino`

### What it is

Interactive serial command tester for direct operation-by-operation checks.

### Best use

Use this when you want to isolate a single failing operation (`writeByte`, `eraseSector`, `readStr`, etc.) without full diagnostics noise.

### Full command map (all interactive commands)

1. `getJEDECID` and component ID bytes
2. `writeByte(addr, byte)`
3. `readByte(addr)`
4. `writeWord(addr, word)`
5. `readWord(addr)`
6. `writeStr(addr, inputString)`
7. `readStr(addr, outputString)`
8. `writeByteArray(addr, pageBuffer, SPI_PAGESIZE)` where `pageBuffer` is `0..255`
9. `readByteArray(addr, data_buffer, SPI_PAGESIZE)` via `printPage`
10. Full-chip dump: loops through flash and calls `readByteArray` repeatedly in `printAllPages`
11. `eraseSector(addr)` (4 KB)
12. `eraseBlock32K(addr)`
13. `eraseBlock64K(addr)`
14. `eraseChip()`

### Tips

- Set Serial Monitor to expected line ending mode from sketch notes.
- Test simple operations first (`getID`, `writeByte` + `readByte`).
- Move to erase and string operations after basics pass.

### Expected serial output (sample)

On boot:

```text
Initialising..........
...
Please pick from the following commands...
1. getID
2. writeByte [address] [byte]
3. readByte [address]
...
14. Erase Chip
```

When running command `1`:

```text
Function 1 : Get JEDEC ID
Manufacturer ID: efh
Memory Type: 40h
Capacity: 17h
JEDEC ID: ef4017h
```

When running `2` then `3` on same address:

```text
Function 2 : Write Byte
... has been written to address ...

Function 3 : Read Byte
The byte at address ... is: ...
```

---

## `getAddressEx/getAddressEx.ino`

### What it teaches

How to use `getAddress()` and `sizeofStr()` to allocate addresses safely without hardcoding offsets.

### Exact API calls used

- Allocation:
  - `flash.getAddress(sizeof(byte))`
  - `flash.getAddress(sizeof(float))`
  - `flash.getAddress(flash.sizeofStr(testStr[i]))`
- Byte I/O:
  - `flash.writeByte(byteAddr[i], testByte[i])`
  - `flash.readByte(byteAddr[i])`
- Float I/O:
  - `flash.writeFloat(floatAddr[i], testFloat[i])`
  - `flash.readFloat(floatAddr[i])`
- String I/O:
  - `flash.writeStr(strAddr[i], testStr[i])`
  - `flash.readStr(strAddr[i], _string)`

### Troubleshooting value

If this fails, you may have pre-written data conflicts or boundary behavior confusion.

### Expected serial output (sample)

```text
Initialising Flash memory..........

Byte Address 0 : 0x...
Byte Address 1 : 0x...
Float Address 0 : 0x...
String Address 0 : 0x...

3 written to 0x...
3 read from 0x...
3.1415 written to 0x...
3.1415 read from 0x...
'Test String 0' written to 0x...
'Test String 0' read from 0x...
```

Addresses should be valid hex values and read-back values should match writes.

---

## `readWriteString/readWriteString.ino`

### What it teaches

Simple `writeStr`/`readStr` workflow.

### Exact API calls used

- `flash.getCapacity()` (for random address generation)
- `flash.writeStr(strAddr, inputString)`
- `flash.readStr(strAddr, outputString)`
- `flash.eraseSector(strAddr)` (cleanup after test)

### Troubleshooting value

Great for catching string-specific issues (length metadata, readback integrity, sector erase assumptions).

### Expected serial output (sample)

```text
Written string: This is a test String
To address: 123456
Read string: This is a test String
From address: 123456
```

The written/read strings should be identical and the address should match in both lines.

---

## `Struct_writer/Struct_writer.ino`

### What it teaches

Write/read complete structs repeatedly, including nested members and arrays.

### Exact API calls used

- `flash.getCapacity()` (random target address range)
- `flash.eraseSection(_addr, sizeof(configurationIn))`
- `flash.writeAnything(_addr, configurationIn)`
- `flash.readAnything(_addr, configurationOut)`

Loop behavior detail:

- Runs `NUMBEROFREPEATS` iterations (`100` by default).
- Tracks `eraseCount`, `writeCount`, `errorCount`, `readCount`.
- Final summary prints all counters.

### Troubleshooting value

Use this for data-model persistence validation. If this fails while primitive tests pass, inspect struct layout assumptions and erase coverage.

### Expected serial output (sample)

```text
Initialising Flash memory..........
1
2
3
...
100

----------------------------------------------------------------
                Final results
----------------------------------------------------------------
No. of successful erases:          100
No. of successful writes:          100
No. of errors generated:           0 (errorCheck function failures)
No. of successful reads:           100
```

Counts should be close to `NUMBEROFREPEATS` (default `100`), with errors at `0`.

---

## `FlashDiagnostics/Diagnostics_functions.ino`

### What it is

Helper routines used by diagnostics output (formatting, timing presentation, pass/fail display).

### Why it matters

Helps you understand what diagnostics output lines map to which underlying test sections.

### Expected serial behavior

This file does not run as a standalone sketch. It provides helper print functions used by `FlashDiagnostics.ino`, including:

- line separators (`printLine`)
- PASS/FAIL formatting (`pass`)
- runtime formatting (`printTimer`, `printTime`)
- ID printing (`getID`, `printUniqueID`)

If formatting looks wrong in diagnostics output, this helper file is the first place to inspect.

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
