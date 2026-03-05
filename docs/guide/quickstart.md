# Quick Start

## Who this is for

If you are new to Arduino external memory chips, start here.

## 1. Install the library

### Arduino Library Manager

1. Open Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries**.
3. Search for **SPIMemory**.
4. Install latest version.

### ZIP install

1. Download repository ZIP.
2. Rename extracted folder to `SPIMemory`.
3. Move it to your Arduino libraries folder.

## 2. Wire the chip

Connect at minimum:

- `SCK`
- `MISO`
- `MOSI`
- `CS` (chip select)
- `VCC`
- `GND`

For flash chips, ensure `HOLD` and `WP` pins are pulled to valid logic levels as required by datasheet.

## 3. First flash sketch

```cpp
#include <SPIMemory.h>

SPIFlash flash(10); // CS pin

void setup() {
  Serial.begin(115200);

  if (!flash.begin()) {
    Serial.print("begin failed, err=0x");
    Serial.println(flash.error(), HEX);
    return;
  }

  uint32_t addr = 0;
  flash.eraseSector(addr);

  flash.writeULong(addr, 123456789UL);
  uint32_t out = flash.readULong(addr);

  Serial.print("Read: ");
  Serial.println(out);
}

void loop() {}
```

## 4. Validate using bundled examples

Recommended beginner order:

1. `examples/FlashDiagnostics/FlashDiagnostics.ino`
2. `examples/readWriteString/readWriteString.ino`
3. `examples/getAddressEx/getAddressEx.ino`
4. `examples/Struct_writer/Struct_writer.ino`

## 5. If it fails

- Call `flash.error(VERBOSE)`.
- Check [Errors and Diagnostics](../api/errors.md).
- Run diagnostics example and capture output.
