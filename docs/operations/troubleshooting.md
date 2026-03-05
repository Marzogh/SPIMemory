# Troubleshooting

## Chip does not respond

- verify wiring and voltage levels
- verify CS pin selection
- lower SPI clock and retest
- ensure `begin()` is called before IO

## Write failures

- erase flash region before writing
- disable `HIGHSPEED` while debugging
- check address boundaries and overflow configuration

## Intermittent behavior

- verify shared SPI bus ownership
- verify board-core transaction compatibility
- enable diagnostics and capture error code sequence
