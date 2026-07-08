# level06

Nothing of note with `ll`.
`checksec` tells us that the `Partial RELRO`, `Canary found`, and `NX enabled` protections are present.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|printf|main|
|strcspn|clear_stdin|
|fflush|prog_timeout|
|getchar|enable_timeout_cons|
|fgets|auth|
|signal||
|alarm||
|puts||
|system||
|strnlen||
|ptrace||
|scanf||

```sh
for i in $(seq 1 40); do (python -c "print('login'); print('18')" | ./level02) | grep do; done;
```

Let code be X. X is in the interval
```math
(32 ^ 0x1337) + 0x5eeded <= X <= (127 ^ 0x1337) + 0x5eeded
so
6226180 <= X <= 6226229

code has at most 31 operations
```
