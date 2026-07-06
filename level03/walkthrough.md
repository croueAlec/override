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

The main program `scanf()`'s an int value from **stdin** and stores it into *user_input*.
*user_input* and the constant `322424845` are passed as arguments to the `test()` function.

The *result* of the subtraction of the two arguments is used as a **switch-case key** that calls the `decrypt()` function using the *result* as argument.

In the `decrypt()` function, each character of the ``Q}|u`sfg~sf{}|a3`` string is `xor`'d using the *result* variable.

If the output string is equal to `Congratulations!`, the shell is opened.

Now we just need to find the number we have to input as password in order to decypher the string above.

Let's just use the [decyphering script](find_cypher_key.c)
```sh
# Outside the VM

cc level03/find_cypher_key.c
./a.out

[...]
18 : Congratulations!
[...]
```

So, we need *result* to have a value of **18**. Then our input will be `322424845 - 18 = 322424827`.

```sh
./level03
***********************************
*		level03		**
***********************************
Password:322424827
$ whoami
level04
$ cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```

Let's go!

```sh
su level04
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```

Did you know some species of bats are frugivores. This means that they mostly feed on fruits. That's neat!
