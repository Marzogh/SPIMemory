# API Reference

This section provides two ways to find what you need:

- **By task** (beginner-friendly)
- **By full function list** (complete call-by-call reference)

## Start by Task

| Task | Recommended calls |
| --- | --- |
| Initialize memory chip | `begin`, `setClock` |
| Check chip identity | `getJEDECID`, `getManID`, `getUniqueID` |
| Find free write location | `getAddress`, `sizeofStr` |
| Write/read one primitive value | `writeByte`/`readByte`, `writeULong`/`readULong`, etc. |
| Write/read strings | `writeStr`, `readStr` |
| Write/read structs | `writeAnything`, `readAnything` |
| Write/read buffers | `writeByteArray`/`readByteArray`, `writeCharArray`/`readCharArray` |
| Erase flash data | `eraseSector`, `eraseBlock32K`, `eraseBlock64K`, `eraseSection`, `eraseChip` |
| Power control | `powerDown`, `powerUp` |
| Suspend/resume flash operation | `suspendProg`, `resumeProg` |
| Diagnose failures | `error`, `error(VERBOSE)`, `functionRunTime` |

## Full References

- [SPIFlash Function Reference](spiflash-reference.md)
- [SPIFram Function Reference](spifram-reference.md)
- [Errors and Diagnostics](errors.md)

## Function Discovery Index

If you're searching by function name, start here:

### Shared SPIFlash/SPIFram families

`begin`, `setClock`, `libver`, `error`, `getManID`, `getJEDECID`, `getUniqueID`, `getAddress`, `sizeofStr`, `getCapacity`, `functionRunTime`, `writeByte`, `readByte`, `writeChar`, `readChar`, `writeShort`, `readShort`, `writeWord`, `readWord`, `writeLong`, `readLong`, `writeULong`, `readULong`, `writeFloat`, `readFloat`, `writeStr`, `readStr`, `writeByteArray`, `readByteArray`, `writeCharArray`, `readCharArray`, `writeAnything`, `readAnything`, `eraseSection`, `eraseChip`, `powerDown`, `powerUp`

### SPIFlash-only families

`sfdpPresent`, `getMaxPage`, `eraseSector`, `eraseBlock32K`, `eraseBlock64K`, `suspendProg`, `resumeProg`
