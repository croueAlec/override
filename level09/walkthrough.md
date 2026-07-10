# level08

Nothing of note with `ll`.
`checksec` tells us that the `Partial RELRO`, `NX enabled` and `PIE enabled` protections are present.
`Partial RELRO` does not prevent a **PLT** overwrite.
`NX enabled`, or **N**o e**X**ecution indicates that it is not possible to execute code from the stack.
`PIE enabled` as tasty as this sounds, **Position Independant Executable** means the binary doesn’t run at a fixed memory address.
At each execution the base address is defined and all function addresses will have this offset.
So `printf address == base address + offset`.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|fgets|main|
|printf|handle_msg|
|puts|save|
|stdin|secret_backdoor|
|strncpy|set_msg|
|system|set_username|

Keen eyes will have spotted the `secret_backdoor()` function.
If we manage to reach it, we will be able to inject any command we want as an argument of `system()`.

We might be able to overflow **buffer[140]** in `handle_message()` to overwrite the return address to `secret_backdoor()`.
