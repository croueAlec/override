# level00

Nothing of note with `ll` and `checksec`.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|strncmp|main|
|puts||
|fread||
|fclose||
|system||
|printf||
|strcspn||
|fgets||
|fopen||
|exit||
|fwrite||

The `main()` function has no `return`, only calls to `exit()` and as such we will not be able to overflow using the return value.

However, there is an unguarded `printf()` at the end of the code.
This means that we can exploit this by printing stack values

```sh
python -c "print('a' * 100 + 'coucou');" | ./level02

for i in $(seq 1 5); do (python -c "print('a' * $i); print('password')" | ./level02); done;
```
