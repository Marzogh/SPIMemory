# SPIFram Constructors

## Signatures

- `SPIFram(uint8_t cs = CS)`
- `SPIFram(uint8_t cs, SPIClass *spiinterface)` (supported boards)

## What It Is For

Construct a `SPIFram` object and choose which SPI bus and chip-select pin are used to access FRAM.

## Parameters

- `cs`: Chip-select pin connected to FRAM chip CS.
- `spiinterface`: Optional bus pointer for multi-SPI boards.

## Behavior Details

Construction only stores interface wiring. Hardware probing, device identification, and readiness checks happen in `begin()`.

## Return Semantics

Constructors do not return values; they prepare the instance.

## Failure Behavior

Incorrect bus/pin choices usually surface during `begin()` with zero IDs or non-zero error codes.

## Common Mistakes

- Using incorrect CS pin or alternate bus for your board wiring.
- Assuming constructor validates wiring immediately (validation happens at `begin()`).
- Reusing examples with different board pin numbering conventions without adjustment.

## Example

```cpp
#include <SPIMemory.h>

SPIFram fram(10);
// SPIFram fram(5, &SPI1);
```
