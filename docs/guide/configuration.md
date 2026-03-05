# Build Configuration

This library is configured mainly through compile-time flags in `src/SPIMemory.h`.

## Compile-Time Flags

| Flag | Default | What it does | When to use |
| --- | --- | --- | --- |
| `USES_SFDP` | Off | Enables SFDP discovery logic for compatible flash chips | Using unsupported but SFDP-capable flash parts |
| `RUNDIAGNOSTIC` | Off | Enables verbose diagnostics messaging and runtime tracing support | Hardware bring-up and debugging |
| `HIGHSPEED` | Off | Skips pre-write blank checks (`_notPrevWritten`) | Performance tuning after you trust erase discipline |
| `DISABLEOVERFLOW` | Off | Disables address wraparound at end-of-chip | Safety-critical logging where wrap is not acceptable |
| `ENABLEZERODMA` | Off | Enables experimental SAMD DMA path | Advanced tuning on SAMD if validated on your board |

## Runtime Configuration Calls

- `begin(chipSize)` for optional explicit capacity override.
- `setClock(clockSpeed)` for SPI speed tuning.
- Alternate constructor variants for non-default SPI bus or custom pins.

## Constructor Guide

### Default SPI

- `SPIFlash flash(csPin);`
- `SPIFram fram(csPin);`

### Alternate SPI interface (supported platforms)

- `SPIFlash flash(csPin, &SPI1);`
- `SPIFram fram(csPin, &SPI1);`

### Custom SPI pins (ESP32-oriented)

- `SPIFlash flash(spiPinsArray);` where array order is `sck, miso, mosi, ss`.

## Recommended beginner defaults

- Keep all compile-time flags at default.
- Keep write `errorCheck=true`.
- Enable `RUNDIAGNOSTIC` only while debugging.
