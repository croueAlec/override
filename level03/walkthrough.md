# level03

Nothing of note with `ll`.
`checksec` on the other hand has a lot to say this time :
`Partial RELRO`, `Canary found`, `NX enabled`, so we cannot overwrite the **GOT**, there is a buffer **overflow protection**, and we cannot **execute shellcode** on the stack.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|printf|frame_dummy*|
|fflush|clear_stdin|
|getchar|get_unum|
|time|prog_timeout*|
|puts|decrypt|
|system|test|
|srand|main|
|rand||
|scanf||

*prog_timeout* and *frame_dummy* were omitted from the [source code](source.c).

