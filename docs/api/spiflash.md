# SPIFlash API

## Functional groups

- Initialization: `begin`, `setClock`, `sfdpPresent`
- Identification: `getJEDECID`, `getUniqueID`, `getCapacity`, `getMaxPage`
- Addressing helpers: `getAddress`, `sizeofStr`
- Read/write primitives and arrays
- Generic IO: `writeAnything`, `readAnything`
- Erase operations: sector/block/chip/section
- Power and suspend/resume

## Behavioral notes

- Writes are page-aware and support page-boundary spanning.
- Write verification is enabled by default.
- Erase timing comes from defaults or SFDP when enabled.
