# suspendProg (SPIFlash)

## Signatures

- `bool SPIFlash::suspendProg(void)`

## What It Is For

Suspend an in-progress erase/program operation on chips that support suspend/resume.

## Parameters

- No parameters.

## Behavior Details

Useful for responsiveness: pause long erase, service urgent reads, then resume.

## Return Semantics

Returns `true` if suspend request succeeded.

## Failure Behavior

Not all chips support suspend semantics for every operation/state.

## Example

```cpp
if (!flash.suspendProg()) {
  flash.error(VERBOSE);
}
```
