# Struct_writer.ino

Sketch path: `examples/Struct_writer/Struct_writer.ino`

## Purpose

Stress-test struct persistence across repeated erase/write/read cycles.

## Exact API Calls Used

- `flash.getCapacity()`
- `flash.eraseSection(_addr, sizeof(configurationIn))`
- `flash.writeAnything(_addr, configurationIn)`
- `flash.readAnything(_addr, configurationOut)`

## Loop Behavior

- runs `NUMBEROFREPEATS` iterations (default `100`)
- tracks `eraseCount`, `writeCount`, `errorCount`, `readCount`
- prints final aggregate summary

## Expected Output (Sample)

```text
Initialising Flash memory..........
1
2
3
...
100

----------------------------------------------------------------
                Final results
----------------------------------------------------------------
No. of successful erases:          100
No. of successful writes:          100
No. of errors generated:           0 (errorCheck function failures)
No. of successful reads:           100
```

## What Success Means

- successful counts are close to `NUMBEROFREPEATS`
- error count stays `0` or near zero (if hardware is stable)

## Common Mistakes

- changing struct layout without migration strategy
- assuming one successful run proves long-term stability
- erasing insufficient bytes when adapting struct size
