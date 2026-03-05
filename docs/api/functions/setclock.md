# setClock

## Signatures

- `void SPIFlash::setClock(uint32_t clockSpeed)` (transaction-capable cores)
- `void SPIFlash::setClock(uint8_t clockdiv)` (legacy cores)
- `void SPIFram::setClock(uint32_t clockSpeed)`

## What It Is For

Set SPI clock rate used for memory operations. Useful for stabilizing marginal wiring or increasing throughput after validation.

## Parameters

- `clockSpeed`: SPI clock in Hz (for boards using SPI transactions).
- `clockdiv`: Legacy divider constant on non-transaction platforms.

## Behavior Details

Use slower clocks for bring-up and long wires; use faster clocks after diagnostics are stable. Clock selection affects both reliability and performance.

## Return Semantics

No return value. Clock setting is stored and used by subsequent memory calls.

## Failure Behavior

Too-fast clocks can cause intermittent read/write mismatches, ID read failures, and random PASS/FAIL behavior in diagnostics.

## Example

```cpp
flash.begin();
flash.setClock(8000000); // 8 MHz as a conservative starting point
```
