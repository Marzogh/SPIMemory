---
name: Bug report
about: Create a report to help us squash 'em bugs!
title: Bug report - *Add short descriptor here*
labels: possible bug
assignees: Marzogh

---

##### Any bug report raised here MUST be submitted according to this template or it will be flagged with 'Not enough information'. No action will be taken till all the prerequisite information is provided. If no information is provided for over a month after the 'Not enough information' label is applied, the issue will be closed.

## Pre-bug report checklist:

Do this checklist before filing an bug report:
- [ ] Is this something you can **debug and fix**? Send a pull request! Bug fixes and documentation fixes are welcome.
- [ ] Is this an idea for a feature? Post it as a Feature request. NOT a bug report.
- [ ] Is this a bug that you cannot fix? Go ahead with filing a bug report below

<hr>

## Bug Report

#### Describe the bug

_Include a clear and concise description of what the bug is._

- Make sure you have run FlashDiagnostics.ino with ``` #define RUNDIAGNOSTICS ``` uncommented in SPIFlash.h. Paste the **relevant sections** of the output from your Serial console when you run FlashDiagnostics.ino.here:
```
```FlashDiagnostics output```
```
- If you have a problem with a particular function, call the `flash.error()` function (after you have made sure you have started up your Serial port with a ``` Serial.begin(BAUD) ``` ). Provide details of the function, the data given to/ expected from the function and the error code here: (**Please repeat this for every function you have an error with**)
        - Function: (e.g. writeByte())
       -  Data: (e.g. 3.14 or Struct)
        - Error code: (e.g. 0x0A)


#### To Reproduce
Provide a **minimal code snippet** example that reproduces the bug (If you're using one of the examples that came with the library, just tell us which one instead of pasting the entire example's code). Please make sure you wrap any code in the proper code blocks like below
```
```CODE HERE```
```

#### Expected behavior
A clear and concise description of what you expected to happen.

#### Screenshots
If applicable, add screenshots to help explain your problem.

#### Wiring diagram
If applicable, add a wiring diagram to help explain your problem.

#### Software environment (please complete the following information):
 - Library version: [e.g. v3.3.0]
 - Arduino IDE version: [e.g. 1.8.9]
 - OS: [e.g. Linux]
 - Micro controller platform: [e.g. Feather M0]
 - Flash/Fram memory module: [e.g. SST25VF064C]

#### Additional context
Add any other context about the problem here.

<hr>

###### DO NOT DELETE OR EDIT anything below this

<hr>

<sub> <b> Note 1: _Make sure to add **all the information needed to understand the bug** so that someone can help. If any essential information is missing we'll add the 'Needs more information' label and close the issue until there is enough information._ </b></sub>

<sub> <b> Note 2: For support questions (for example, tutorials on how to use the library), please use the [Arduino Forums](http://forum.arduino.cc/index.php?topic=324009.0). This repository's issues are reserved for feature requests and bug reports. </b></sub>

<hr>

![GitHub issue state](https://img.shields.io/github/issues/detail/s/Marzogh/SPIFlash/14.svg) ![GitHub issue title](https://img.shields.io/github/issues/detail/title/Marzogh/SPIFlash/14.svg) ![GitHub issue author](https://img.shields.io/github/issues/detail/u/Marzogh/SPIFlash/14.svg) ![GitHub issue label](https://img.shields.io/github/issues/detail/label/Marzogh/SPIFlash/14.svg) ![GitHub issue comments](https://img.shields.io/github/issues/detail/comments/Marzogh/SPIFlash/14.svg) ![GitHub issue age](https://img.shields.io/github/issues/detail/age/Marzogh/SPIFlash/14.svg) ![GitHub issue last update](https://img.shields.io/github/issues/detail/last-update/Marzogh/SPIFlash/14.svg)

