# Build Configuration

## Compile-Time Flags (in `SPIMemory.h`)

- `USES_SFDP`: enable SFDP discovery for unsupported flash chips
- `RUNDIAGNOSTIC`: verbose diagnostics output
- `HIGHSPEED`: skip pre-write blank checks
- `DISABLEOVERFLOW`: disable address rollover
- `ENABLEZERODMA`: experimental SAMD DMA mode

## Runtime Configuration

- `begin(chipSize)` optionally forces chip size
- `setClock(clockSpeed)` changes SPI clock settings
- platform-specific constructors enable alternate SPI interfaces

## Platform Notes

When default SPI pins/bus are unsuitable, use board-specific constructor variants.
