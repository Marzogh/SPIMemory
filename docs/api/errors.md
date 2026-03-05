# Errors and Diagnostics

## How to read errors

- `error()` returns the last error code as a byte.
- `error(VERBOSE)` prints a message to serial and still returns the code.

## Error Code Table

These values come from `src/diagnostics.h`.

| Code | Name | Meaning |
| --- | --- | --- |
| `0x00` | `SUCCESS` | Function completed successfully |
| `0x01` | `CALLBEGIN` | `begin()` was not called |
| `0x02` | `UNKNOWNCHIP` | Chip ID not recognized |
| `0x03` | `UNKNOWNCAP` | Capacity not identified |
| `0x04` | `CHIPBUSY` | Chip remained busy past timeout |
| `0x05` | `OUTOFBOUNDS` | Address exceeded memory limits with overflow disabled |
| `0x06` | `CANTENWRITE` | Write-enable latch could not be set |
| `0x07` | `PREVWRITTEN` | Target area already contains data |
| `0x08` | `LOWRAM` | Low RAM condition detected |
| `0x09` | `SYSSUSPEND` | Suspend/resume state error |
| `0x0A` | `ERRORCHKFAIL` | Post-write verification failed |
| `0x0B` | `NORESPONSE` | No chip response over SPI |
| `0x0C` | `UNSUPPORTEDFUNC` | Function unsupported by chip |
| `0x0D` | `UNABLETO4BYTE` | Could not enable 4-byte addressing |
| `0x0E` | `UNABLETO3BYTE` | Could not return to 3-byte addressing |
| `0x0F` | `CHIPISPOWEREDDOWN` | Operation attempted while powered down |
| `0x10` | `NOSFDP` | SFDP not available |
| `0x11` | `NOSFDPERASEPARAM` | SFDP erase parameters unavailable |
| `0x12` | `NOSFDPERASETIME` | SFDP erase timing unavailable |
| `0x13` | `NOSFDPPROGRAMTIMEPARAM` | SFDP program timing unavailable |
| `0x14` | `NOCHIPSELECTDECLARED` | Custom SPI constructor missing chip select |
| `0xFE` | `UNKNOWNERROR` | Unknown error state |

## Fast Debug Sequence (Beginners)

1. Call `begin()` in `setup()` and print `getJEDECID()`.
2. If zero or invalid, check wiring and chip-select pin.
3. Call `error(VERBOSE)` after each failed operation.
4. For flash writes, erase region before writing.
5. Keep `errorCheck=true` until your setup is stable.

## Best Practices

- Keep a small serial log of operation + returned error code.
- For production firmware, map key error codes into telemetry counters.
- Use `FlashDiagnostics` example sketch for initial bring-up.
