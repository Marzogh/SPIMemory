# getAddressEx.ino

Sketch path: `examples/getAddressEx/getAddressEx.ino`

## Purpose

Demonstrate safe dynamic address allocation for mixed payload types.

## Exact API Calls Used

Allocation:

- `flash.getAddress(sizeof(byte))`
- `flash.getAddress(sizeof(float))`
- `flash.getAddress(flash.sizeofStr(testStr[i]))`

Byte I/O:

- `flash.writeByte(byteAddr[i], testByte[i])`
- `flash.readByte(byteAddr[i])`

Float I/O:

- `flash.writeFloat(floatAddr[i], testFloat[i])`
- `flash.readFloat(floatAddr[i])`

String I/O:

- `flash.writeStr(strAddr[i], testStr[i])`
- `flash.readStr(strAddr[i], _string)`

## Expected Output (Sample)

```text
Initialising Flash memory..........

Byte Address 0 : 0x...
Byte Address 1 : 0x...
Float Address 0 : 0x...
String Address 0 : 0x...

3 written to 0x...
3 read from 0x...
3.1415 written to 0x...
3.1415 read from 0x...
'Test String 0' written to 0x...
'Test String 0' read from 0x...
```

## What Success Means

- addresses are valid and non-overlapping for requested sizes
- every readback value matches the corresponding write value

## Common Mistakes

- not erasing regions when adapting this flow for flash updates
- mixing manual addresses with allocator addresses
- using `String.length()` instead of `sizeofStr()` for storage planning
