# API Reference

This section supports both quick navigation and deep call-by-call documentation.

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

## Reference Entry Points

- [SPIFlash Function Reference](spiflash-reference.md)
- [SPIFram Function Reference](spifram-reference.md)
- [Function Pages Index (one page per function)](functions/index.md)
- [Errors and Diagnostics](errors.md)

## Search Tip

Use the docs search with the exact function name (for example `writeByteArray`) to jump directly to that function's dedicated page.
