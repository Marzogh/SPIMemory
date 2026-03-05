# Compatibility Matrix

This page lists the compatibility data currently documented in this repository (`README.md`, `library.properties`, and headers).

## Arduino Architectures (from `library.properties`)

- `avr`
- `sam`
- `samd`
- `esp8266`
- `esp32`
- `Simblee`
- `stm32`
- `nrf52`

## Boards and MCUs (documented as tested)

| MCU / family | Example boards tested | Notes |
| --- | --- | --- |
| ATmega328P | Uno, Micro, Fio, Nano | Uses default SPI constructor by default |
| ATmega32u4 | Leonardo, Fio v3 | Wait for `Serial` before logging on boards that require it |
| ATmega2560 | Mega | Standard SPI use |
| ATSAMD21G18 | Feather M0, Feather M0 Express, ItsyBitsy M0 Express | Alternate SPI interface constructor supported |
| AT91SAM3X8E | Arduino Due | Dedicated SAM path in library |
| nRF52832 | Adafruit nRF52 Feather | Included in supported architectures |
| ATSAMD51J19 | Adafruit Metro M4 | Included in tested list |
| STM32F091RCT6 | Nucleo-F091RC | STM32 architecture macro path |
| STM32L0 | Nucleo-L031K6 | Added in v3.4.0 changelog |
| ESP8266 | Adafruit Feather ESP8266, Sparkfun ESP8266 Thing | Uses ESP8266 architecture branch |
| ESP32 | Adafruit Feather ESP32, Sparkfun ESP32 Thing | May require explicit CS and custom SPI pins |
| Simblee | Sparkfun Simblee | Listed as tested |

## Flash Chip Compatibility (documented as tested)

| Manufacturer | Parts listed in repo docs/changelog |
| --- | --- |
| Winbond | W25Q16BV, W25Q64FV, W25Q64JV, W25Q80BV, W25Q256FV |
| Microchip | SST25VF064C, SST26VF016B, SST26VF032B, SST26VF064B |
| Cypress/Spansion | S25FL032P, S25FL116K, S25FL127S |
| ON Semiconductor | LE25U40CMC |
| AMIC | A25L512A0 |
| Micron | M25P40 |
| Adesto | AT25SF041 |
| Macronix | MX25L4005, MX25L8005 |
| GigaDevice | GD25Q16C |

### SFDP note

The library can support more flash chips through SFDP discovery when `USES_SFDP` is enabled in `SPIMemory.h`.

## FRAM Compatibility (documented as tested)

| Manufacturer | Part |
| --- | --- |
| Cypress/Spansion | FM25W256 |

## Constructor Compatibility Notes

- `SPIFlash(uint8_t cs)` and `SPIFram(uint8_t cs)` are the default constructors.
- `SPIFlash(uint8_t cs, SPIClass *spiinterface)` and `SPIFram(uint8_t cs, SPIClass *spiinterface)` are used on multi-SPI platforms.
- `SPIFlash(int8_t *SPIPinsArray)` is intended for custom SPI pin routing (ESP32-style use cases).

## Before You Buy a New Chip

1. Check if the manufacturer is already in the tested list.
2. Check if the part is SFDP-compatible (for flash).
3. Plan to validate with `FlashDiagnostics` example and error codes on your target board.
