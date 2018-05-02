.. _Compatibility:

##############
Compatibility
##############

**********************************************
Arduino IDEs supported (actually tested with)
**********************************************
- IDE v1.5.x
- IDE v1.6.0-v1.6.5
- IDE v1.6.9-v1.6.12
- IDE v1.8.1-v1.8.5

Boards
=======

Completely supported
----------------------
- Arduino Uno
- Arduino Leonardo
- Arduino Due
- Arduino Zero
- Nucleo-F091RC
- Adafruit Feather M0
- Adafruit Feather M0 Express
- ESP8266 Boards (On the Arduino IDE)
- Simblee Boards (On the Arduino IDE)
- Arduino Mega
- Arduino Micro
- Arduino Fio

In BETA
------------
- ESP32 Boards (Tested on the Adafruit ESP32 Feather) The library is known to work with the ESP32 core as of the current commit `25dff4f <https://github.com/espressif/arduino-esp32/tree/25dff4f044151f7f766c64b9d2ad90398472e6b3>`_ on 05.04.2018. [#f1]_ [#f2]_

- Adafruit Metro M4 - more testing required before support for this board leaves beta.

Flash memory chips
===================

Completely supported (Actually tested with)
--------------------------------------------
- Winbond

  - W25Q16BV
  - W25Q64FV
  - W25Q80BV
  - W25Q256FV
- Microchip

  - SST25VF064C
  - SST26VF064B
- Cypress/Spansion

  - S25FL032P
  - S25FL116K
  - S25FL127S
- ON Semiconductor

  - LE25U40CMC
- AMIC

  - A25L512A0
- Micron

  - M25P40
- Adesto

  - AT25SF041

Should work with (Similar enough to the ones actually tested with)
--------------------------------------------------------------------
- Winbond (All SPI Flash chips)
- Microchip (SST25 & SST26 series)
- Cypress/Spansion (S25FL series)
- Any flash memory that is compatible with the SFDP standard as defined in JESD216B

.. rubric:: Footnotes
.. [#f1] ESP32 support will remain in beta till the ESP32 core can be installed via the Arduino boards manager.
.. [#f2] ESP32 boards usually have an SPI Flash already attached to their SS pin, so the user has to declare the ChipSelect pin being used when the constructor is declared - for example: ``SPIFlash flash(33);``
