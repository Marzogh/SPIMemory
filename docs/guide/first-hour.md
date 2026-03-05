# First Hour Setup (Arduino Beginner Path)

This walkthrough is for first-time users bringing up an external SPI memory chip.

## 0-10 min: Hardware sanity check

At minimum, connect:

- `VCC` and `GND`
- `SCK`, `MISO`, `MOSI`
- `CS` (chip select)

### Flash-specific pin note

Many flash chips also expose `WP` (write protect) and `HOLD`. If these are floating, writes can fail or communication can be unstable. Check your chip datasheet and set valid levels.

### ESP32 note

Some ESP32 boards already use onboard flash on default SPI pins. Use an explicit CS pin and, if needed, the custom pin constructor.

## 10-20 min: Minimal identity test

Upload a minimal sketch that does:

1. `begin()`
2. `getJEDECID()`
3. `error(VERBOSE)` on failure

```cpp
#include <SPIMemory.h>

SPIFlash flash(10);

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!flash.begin()) {
    Serial.print("begin failed, err=0x");
    Serial.println(flash.error(VERBOSE), HEX);
    return;
  }

  Serial.print("JEDEC: 0x");
  Serial.println(flash.getJEDECID(), HEX);
}

void loop() {}
```

If JEDEC is zero or invalid, stop and re-check wiring and CS pin.

## 20-35 min: First write/read

For flash:

1. Erase region (`eraseSector`)
2. Write one value (`writeULong`)
3. Read back (`readULong`)

```cpp
uint32_t addr = 0;
flash.eraseSector(addr);
flash.writeULong(addr, 123456789UL);
uint32_t out = flash.readULong(addr);
```

## 35-50 min: Run diagnostics sketch

Run `examples/FlashDiagnostics/FlashDiagnostics.ino`.

- This validates many read/write/erase and power operations.
- It is the fastest way to separate hardware issues from application-code issues.

## 50-60 min: Move to your real use case

Pick one:

- Logging numbers: use primitive write/read calls.
- Saving settings: use `writeAnything/readAnything` with a struct.
- Storing user text: use `writeStr/readStr`.

Then use [Common Tasks](common-tasks.md) and [Example Sketches Guide](example-sketches.md).
