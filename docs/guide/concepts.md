# Core Concepts

## Addressing

All APIs use a 32-bit linear address model.

## Erase Before Write (Flash)

Flash writes require erased memory (typically `0xFF`).
Use erase APIs before writing to previously used regions.

## Overflow

By default, operations can roll over from end-of-chip back to `0x00`.
Define `DISABLEOVERFLOW` to make out-of-bounds fail instead.

## Performance and Safety

- `fastRead=true` can speed reads on supported chips.
- Write functions validate data by default; disabling checks increases speed and risk.

## Flash vs FRAM

- Flash: erase required, generally larger capacity.
- FRAM: no erase needed for byte writes, lower write latency.
