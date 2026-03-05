# getCapacity

## Signatures

- `uint32_t SPIFlash::getCapacity(void)`
- `uint32_t SPIFram::getCapacity(void)`

## What It Is For

Return total chip capacity in bytes used for bounds checks, random-address generation, and buffer planning.

## Parameters

- No parameters.

## Behavior Details

Use this when calculating safe address ranges: valid span is `0` to `capacity - 1`.

## Return Semantics

Returns capacity in bytes.

## Failure Behavior

Using hardcoded capacities causes address overrun on different chip variants.

## Common Mistakes

- Hardcoding capacity from datasheet instead of querying runtime value.
- Using random address upper bound equal to capacity without considering payload size.
- Assuming same capacity across all user hardware variants.

## Example

```cpp
uint32_t cap = flash.getCapacity();
uint32_t addr = random(0, cap);
```
