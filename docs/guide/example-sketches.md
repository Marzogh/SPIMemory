# Example Sketches Guide (Troubleshooting + Learning)

The bundled examples are one of the best parts of this library. This page explains what each one does, what success looks like, and what failures mean.

## `FlashDiagnostics/FlashDiagnostics.ino`

### What it tests

- Chip identity (`getJEDECID`, capacity, max page, unique ID)
- Power operations (`powerDown`, `powerUp`)
- Erase operations (`eraseChip`, `eraseSection`, sector/block erase)
- Data I/O for many types (byte, char, word, short, long, float, struct, array, string)

### Why run it first

It gives broad signal quickly: if this sketch fails, fix hardware/config before writing app logic.

### Expected success signs

- JEDEC ID prints non-zero.
- Function tests report pass.
- Read values match written values.

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

### Tips

- Set Serial Monitor to expected line ending mode from sketch notes.
- Test simple operations first (`getID`, `writeByte` + `readByte`).
- Move to erase and string operations after basics pass.

---

## `getAddressEx/getAddressEx.ino`

### What it teaches

How to use `getAddress()` and `sizeofStr()` to allocate addresses safely without hardcoding offsets.

### Troubleshooting value

If this fails, you may have pre-written data conflicts or boundary behavior confusion.

---

## `readWriteString/readWriteString.ino`

### What it teaches

Simple `writeStr`/`readStr` workflow.

### Troubleshooting value

Great for catching string-specific issues (length metadata, readback integrity, sector erase assumptions).

---

## `Struct_writer/Struct_writer.ino`

### What it teaches

Write/read complete structs repeatedly, including nested members and arrays.

### Troubleshooting value

Use this for data-model persistence validation. If this fails while primitive tests pass, inspect struct layout assumptions and erase coverage.

---

## `FlashDiagnostics/Diagnostics_functions.ino`

### What it is

Helper routines used by diagnostics output (formatting, timing presentation, pass/fail display).

### Why it matters

Helps you understand what diagnostics output lines map to which underlying test sections.

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
