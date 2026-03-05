# Common Tasks (Beginner Friendly)

## Save one number and read it back

```cpp
uint32_t addr = 0;
flash.eraseSector(addr);
flash.writeULong(addr, 42);
uint32_t out = flash.readULong(addr);
```

## Save and load a string

```cpp
String in = "hello";
String out;
uint32_t addr = flash.getAddress(flash.sizeofStr(in));
flash.eraseSection(addr, flash.sizeofStr(in));
flash.writeStr(addr, in);
flash.readStr(addr, out);
```

## Save and load a struct

```cpp
struct Config { uint16_t a; float b; } cfgIn{7, 3.14f}, cfgOut;
uint32_t addr = flash.getAddress(sizeof(Config));
flash.eraseSection(addr, sizeof(Config));
flash.writeAnything(addr, cfgIn);
flash.readAnything(addr, cfgOut);
```

## Write/read byte buffers

```cpp
uint8_t tx[8] = {1,2,3,4,5,6,7,8};
uint8_t rx[8] = {0};
uint32_t addr = flash.getAddress(sizeof(tx));
flash.eraseSection(addr, sizeof(tx));
flash.writeByteArray(addr, tx, sizeof(tx));
flash.readByteArray(addr, rx, sizeof(rx));
```

## Chip not responding

1. Verify wiring and CS pin.
2. Ensure `begin()` is called in `setup()`.
3. Print `getJEDECID()` and `error(VERBOSE)`.
4. Run `FlashDiagnostics` example sketch.

## Improve speed safely

- Read path: use `fastRead=true` selectively.
- Write path: only disable error checks after full validation.
