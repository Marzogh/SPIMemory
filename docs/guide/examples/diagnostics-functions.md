# Diagnostics_functions.ino

Sketch path: `examples/FlashDiagnostics/Diagnostics_functions.ino`

## Purpose

Provide helper routines consumed by `FlashDiagnostics.ino`.

## What This File Defines

Formatting helpers:

- `printLine`
- `printTab`
- `printTime`
- `printTimer`
- `pass`

Identity and metadata helpers:

- `printUniqueID`
- `getID`

Test routines called by `FlashDiagnostics.ino`:

- power: `powerDownTest`, `powerUpTest`
- erase: `eraseChipTest`, `eraseSectionTest`, `eraseBlock64KTest`, `eraseBlock32KTest`, `eraseSectorTest`
- data: `byteTest`, `charTest`, `wordTest`, `shortTest`, `uLongTest`, `longTest`, `floatTest`, `structTest`, `arrayTest`, `stringTest`

## Important Note

This is not intended as a standalone sketch to upload directly. It is a companion source file used by `FlashDiagnostics.ino`.

## When To Read This Page

- diagnostics output formatting looks wrong
- you need to map a failing table row back to exact test routine
- you are extending diagnostics with new data-type checks
