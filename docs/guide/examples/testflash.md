# TestFlash.ino

Sketch path: `examples/TestFlash/TestFlash.ino`

## Purpose

Interactive serial test console for operation-by-operation validation and isolation.

## Full Command Map

1. `getJEDECID` and ID bytes
2. `writeByte(addr, byte)`
3. `readByte(addr)`
4. `writeWord(addr, word)`
5. `readWord(addr)`
6. `writeStr(addr, inputString)`
7. `readStr(addr, outputString)`
8. `writeByteArray(addr, pageBuffer, SPI_PAGESIZE)` (`0..255` pattern)
9. `readByteArray(addr, data_buffer, SPI_PAGESIZE)` via `printPage`
10. full-chip dump via repeated `readByteArray`
11. `eraseSector(addr)` (4 KB)
12. `eraseBlock32K(addr)`
13. `eraseBlock64K(addr)`
14. `eraseChip()`

## Best Usage Pattern

1. Run command `1` first to verify comms.
2. Validate simple write/read (`2` + `3`).
3. Move to string tests (`6` + `7`).
4. Use erase commands (`11` to `14`) only when needed.

## Expected Output (Sample)

Boot:

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

Command `1`:

```text
Function 1 : Get JEDEC ID
Manufacturer ID: efh
Memory Type: 40h
Capacity: 17h
JEDEC ID: ef4017h
```

Command `2` then `3` on same address:

```text
Function 2 : Write Byte
... has been written to address ...

Function 3 : Read Byte
The byte at address ... is: ...
```

## Common Mistakes

- Serial monitor line-ending mode not matching sketch expectations
- testing advanced erase/full-dump before basic read/write passes
- using wrong address assumptions near capacity boundary
