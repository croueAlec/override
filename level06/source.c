#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ptrace.h>

/* Decompiled using Ghidra */

int main(void)
{
	char	login[32];
	size_t	serial;
	
	puts("***********************************");
	puts("*\t\tlevel06\t\t	*");
	puts("***********************************");
	printf("-> Enter Login: ");
	fgets(login, 32, stdin);
	puts("***********************************");
	puts("***** NEW ACCOUNT DETECTED ********");
	puts("***********************************");
	printf("-> Enter Serial: ");
	scanf("%u", &serial);

	if (auth(login, serial) == 0) {
		puts("Authenticated!");
		system("/bin/sh");
	}

	return 0;
}

int auth(char *login, size_t serial)
{
	int	length;
	size_t	code;

	length = strcspn(login, "\n");	// This function finds the length of argument 1 until the first occurence of any character in argument 2 or until end of the string
	login[length] = '\0';
	length = strnlen(login, 32);	// returns the length of arg1 or arg2, whichever is shorter
	if (length < 6) {
		return 1;
	} else {

		if (ptrace(PTRACE_TRACEME) == -1) {					// This function checks if it is traced, for instance by GDB
			puts("\x1b[32m.---------------------------.");
			puts("\x1b[31m| !! TAMPERING DETECTED !!	|");
			puts("\x1b[32m\'---------------------------\'");
			return 1;
		} else {
			code = (login[3] ^ 0x1337) + 0x5eeded;
			for (int i = 0; i < length; i = i + 1) {
				if (login[i] < ' ') {					// checks for printable characters only
					return 1;
				}
				code = code + (login[i] ^ code) % 1337;
			}

			if (serial == code) {
				return 0;				// authenticated
			} else {
				return 1;				// error
			}
		}
	}
}

/* === The functions below are not used === */

void enable_timeout_cons(void)
{
	signal(0xe,prog_timeout);
	alarm(0x3c);
	return;
}

void clear_stdin(void)
{
	int iVar1;

	do {
		iVar1 = getchar();
		if ((char)iVar1 == '\n') {
			return;
		}
	} while ((char)iVar1 != -1);
	return;
}

int get_unum(void)
{
	int local_10 [3];

	local_10[0] = 0;
	fflush(stdout);
	scanf(&DAT_08048a60,local_10);
	clear_stdin();
	return local_10[0];
}

void prog_timeout(void)
{
	code *pcVar1;

	pcVar1 = (code *)swi(0x80);
	(*pcVar1)();
	return;
}
