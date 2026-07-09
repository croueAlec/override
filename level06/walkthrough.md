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

We once again have a level where we need to 'solve' the input.
Unfortunately, there is a protection against debuggers in the code.

From the `man 2 ptrace`
```
[...]
DESCRIPTION
       The  ptrace()  system  call  provides  a means by which one process (the
       "tracer") may observe and control the execution of another process
[...]
PTRACE_TRACEME
              Indicate  that  this  process  is  to be traced by its parent.
[...]
```

What this means is that `if (ptrace(PTRACE_TRACEME) == -1)` checks if the program is monitored by it's parent.
GDB runs programs as children while monitoring them, thus if we tried to run `gdb ./level06`, it would give us this

```sh
gdb -q ./level06 
Reading symbols from /home/users/level06/level06...(no debugging symbols found)...done.
(gdb) r
Starting program: /home/users/level06/level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: acroue
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6767
.---------------------------.
| !! TAMPERING DETECTED !!  |
'---------------------------'
[Inferior 1 (process 1929) exited with code 01]
(gdb) quit
```

So in order to print the value of *code* that matches the *login* we used as input, we would need to bypass this check. (This check also never restores terminal color to default so we need to exit the shell in order to restore it manually lol)


We need to do two things here, break before the `ptrace()` comparison and change it's return value, and break before `auth()`'s return to print the value of *code* for our specific *login*.

```sh
gdb ./level06
(gdb) disass auth
[...]
   0x080487ba <+114>:	cmp    $0xffffffff,%eax		# This is the comparison that checks for tampering. With $eax being the return value of ptrace
[...]
   0x08048866 <+286>:	cmp    -0x10(%ebp),%eax		# This is the comparison between`serial` (our input password) and code (the processed value of our login)
[...]

(gdb) b *0x080487ba
Breakpoint 1 at 0x80487ba		# ptrace breakpoint
(gdb) b *0x08048866
Breakpoint 2 at 0x8048866		# code comparison breakpoint
(gdb) r
Starting program: /home/users/level06/level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: acroue					# user login
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6767					# user serial

Breakpoint 1, 0x080487ba in auth ()
(gdb) p $eax
$1 = -1
(gdb) set variable $eax = 42			# changing the return value of ptrace to anything other than -1
(gdb) c
Continuing.

Breakpoint 2, 0x08048866 in auth ()
(gdb) p $eax
$2 = 6767								# the current value of our serial
(gdb) x/wu $ebp - 0x10
0xffffd6a8:	6232805						# the processed value of the user login `acroue`
```

Let's input our *login* and the *code* we found.

```sh
./level06 
***********************************
*		level06		  *
***********************************
-> Enter Login: acroue
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6232805
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

Let's go!!
```sh
su level07
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

Btw I spent 30 minutes trying this method with the login *alec* having forgotten it needed to be 6 chars long...
