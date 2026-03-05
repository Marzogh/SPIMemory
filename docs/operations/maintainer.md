# Maintainer Guide

## Critical code paths

- `SPIFlash::_prep` and `_addressCheck`
- `SPIFlash::_chipID` and SFDP branch
- architecture-specific SPI transaction start/end
- FRAM identification and capacity derivation

## Adding chip support

- update ID/capacity detection
- define erase/program behavior and timing
- validate read/write/erase/power flows on hardware
- update docs and examples in same change

## Regression focus

- page-boundary writes
- overflow behavior
- suspend/resume semantics
- power-down/power-up semantics
