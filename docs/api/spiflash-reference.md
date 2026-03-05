# SPIFlash Function Reference

This is the SPIFlash call index. Every function below links to its own detailed page.

## Constructors

- [`SPIFlash(uint8_t cs = CS)`](functions/constructors-spiflash.md)
- [`SPIFlash(uint8_t cs, SPIClass *spiinterface)`](functions/constructors-spiflash.md)
- [`SPIFlash(int8_t *SPIPinsArray)`](functions/constructors-spiflash.md)

## Initialization and Chip Info

- [`begin`](functions/begin.md)
- [`setClock`](functions/setclock.md)
- [`libver`](functions/libver.md)
- [`sfdpPresent`](functions/sfdppresent.md)
- [`error`](functions/error.md)
- [`getManID`](functions/getmanid.md)
- [`getJEDECID`](functions/getjedecid.md)
- [`getUniqueID`](functions/getuniqueid.md)
- [`getAddress`](functions/getaddress.md)
- [`sizeofStr`](functions/sizeofstr.md)
- [`getCapacity`](functions/getcapacity.md)
- [`getMaxPage`](functions/getmaxpage.md)
- [`functionRunTime`](functions/functionruntime.md)

## Single-Value I/O

- [`writeByte`](functions/writeByte.md)
- [`readByte`](functions/readByte.md)
- [`writeChar`](functions/writeChar.md)
- [`readChar`](functions/readChar.md)
- [`writeShort`](functions/writeShort.md)
- [`readShort`](functions/readShort.md)
- [`writeWord`](functions/writeWord.md)
- [`readWord`](functions/readWord.md)
- [`writeLong`](functions/writeLong.md)
- [`readLong`](functions/readLong.md)
- [`writeULong`](functions/writeULong.md)
- [`readULong`](functions/readULong.md)
- [`writeFloat`](functions/writeFloat.md)
- [`readFloat`](functions/readFloat.md)
- [`writeStr`](functions/writeStr.md)
- [`readStr`](functions/readStr.md)

## Buffer I/O

- [`writeByteArray`](functions/writeByteArray.md)
- [`readByteArray`](functions/readByteArray.md)
- [`writeCharArray`](functions/writeCharArray.md)
- [`readCharArray`](functions/readCharArray.md)

## Generic I/O

- [`writeAnything`](functions/writeAnything.md)
- [`readAnything`](functions/readAnything.md)

## Erase Operations

- [`eraseSection`](functions/eraseSection.md)
- [`eraseSector`](functions/eraseSector.md)
- [`eraseBlock32K`](functions/eraseBlock32k.md)
- [`eraseBlock64K`](functions/eraseBlock64k.md)
- [`eraseChip`](functions/eraseChip.md)

## Program and Power State

- [`suspendProg`](functions/suspendProg.md)
- [`resumeProg`](functions/resumeProg.md)
- [`powerDown`](functions/powerDown.md)
- [`powerUp`](functions/powerUp.md)

## All Function Pages

- [Function Pages Index](functions/index.md)
