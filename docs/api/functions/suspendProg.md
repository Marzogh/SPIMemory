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

## Common Mistakes

- Assuming suspend is supported on every chip and operation state.
- Calling suspend/resume without checking return values.
- Suspending and never resuming, leaving long operation incomplete.

## Choosing Between Similar APIs

- Use `suspendProg()` only during long erase/program operations when urgent access is needed.
- Use normal blocking flow if your application can tolerate erase latency.
- Always pair with `resumeProg()` when suspend succeeds.

## Example

```cpp
if (!flash.suspendProg()) {
  flash.error(VERBOSE);
}
```
