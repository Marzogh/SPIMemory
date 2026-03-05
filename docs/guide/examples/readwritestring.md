# readWriteString.ino

Sketch path: `examples/readWriteString/readWriteString.ino`

## Purpose

Show minimal string write/read flow in a single run.

## Exact API Calls Used

- `flash.getCapacity()`
- `flash.writeStr(strAddr, inputString)`
- `flash.readStr(strAddr, outputString)`
- `flash.eraseSector(strAddr)`

## Expected Output (Sample)

```text
Written string: This is a test String
To address: 123456
Read string: This is a test String
From address: 123456
```

## What Success Means

- read string exactly equals written string
- read and write address lines match

## Common Mistakes

- using a different address for readback than write
- not reserving enough space when adapting to variable strings
- assuming this example validates struct/binary layouts (it is string-only)
