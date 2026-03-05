# SPIFlash Constructors

## Signatures

- `SPIFlash(uint8_t cs = CS)`
- `SPIFlash(uint8_t cs, SPIClass *spiinterface)` (supported boards)
- `SPIFlash(int8_t *SPIPinsArray)` (non-standard SPI pin mapping)

## What It Is For

Construct a `SPIFlash` object and define which chip-select pin and SPI bus/pins are used to talk to the flash chip.

## Parameters

- `cs`: Chip-select pin used for the memory chip.
- `spiinterface`: Optional SPI bus object (`&SPI`, `&SPI1`, etc.) on boards with multiple buses.
- `SPIPinsArray`: Non-standard SPI pin mapping array (`sck`, `miso`, `mosi`, `ss`) for supported targets.

## Behavior Details

Constructor selection is a board-integration decision: use default constructor on standard wiring, alternate bus constructor when sharing buses, and custom pin constructor when your board supports remapped SPI pins. Construction does not probe the chip yet; `begin()` does that.

## Return Semantics

Constructors do not return values. They create the object and store interface configuration for later calls.

## Failure Behavior

Misconfigured pins or bus selection are usually detected at `begin()` time via failed ID reads or error codes.

## Example

```cpp
#include <SPIMemory.h>

SPIFlash flash(10);              // default SPI bus, CS on D10
// SPIFlash flash(5, &SPI1);     // alternate SPI bus on supported boards
// int8_t pins[4] = {18, 19, 23, 5};
// SPIFlash flash(pins);         // custom SPI pin mapping on supported targets
```
