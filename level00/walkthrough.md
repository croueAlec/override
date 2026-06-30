# level00

Nothing of note with `ll`.
`checksec` shows that **Partial RELRO** and **NX** are enabled.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|printf||
|puts|main|
|system|frame_dummy|
|scanf||

There is always a `frame_dummy()` function that does nothing. Going forward, I will be ignoring it unless it does something.

The `main()` function calls to `scanf()` and puts the result in `local_14`, an *int array*. If the first index equals **5276** we access the `system()` function that opens a shell for us.

Let's do just that then
```sh
./level00
***********************************
* 	     -Level00 -		  *
***********************************
Password:5276

Authenticated!
$ whoami
level01
$ cat /home/users/level01/.pass
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
$ su level01
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
```

Let the games begin!
