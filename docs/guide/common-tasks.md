# Common Tasks (Beginner Friendly)

## Save one number and read it back

Use `writeULong` and `readULong`.

## Save a text string

Use `writeStr` and `readStr`.

## Save a custom struct

Use `writeAnything` and `readAnything`.

## Find where to write next

Use `getAddress(size)` before writing.

## Erase enough flash for a variable-sized record

Use `eraseSection(address, size)`.

## Chip not responding

1. Confirm wiring and CS pin.
2. Call `begin()` in `setup()`.
3. Read `error(VERBOSE)`.
4. Enable `RUNDIAGNOSTIC` during debug.

## Speed up reads/writes

- Reads: use `fastRead=true` where needed.
- Writes: disable error check only after validation.
