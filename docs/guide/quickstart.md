# Quick Start

## Goal

Initialize a flash chip and verify one write/read cycle.

## Checklist

- Wire SPI pins correctly (SCK, MISO, MOSI, CS, VCC, GND)
- Instantiate `SPIFlash` with the desired chip-select pin
- Call `begin()` in `setup()`
- Erase the target region before first flash write

## Minimal Sketch

```cpp
#include <SPIMemory.h>

SPIFlash flash(10);

void setup() {
  Serial.begin(115200);
  flash.begin();

  const uint32_t addr = 0;
  flash.eraseSector(addr);
  flash.writeULong(addr, 0xDEADBEEF);

  uint32_t out = flash.readULong(addr);
  Serial.println(out, HEX);
}

void loop() {}
```

## Next Steps

- Use `getAddress()` for automatic allocation.
- Use typed APIs and `writeAnything`/`readAnything` for structs.
- Enable diagnostics during bring-up and hardware debug.
