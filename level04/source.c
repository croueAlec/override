#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <linux/prctl.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <string.h>

/* Decompiled using Ghidra */

int main(void)
{
	int		iVar1;
	uint	local_a4;
	char	local_a0[128];
	long	local_18;
	pid_t	pid;

	pid = fork();
	bzero(local_a0, 128);
	local_18 = 0;
	local_a4 = 0;
	if (pid == 0) {
		prctl(1, 1);
		ptrace(PTRACE_TRACEME, 0, 0, 0);
		puts("Give me some shellcode, k");
		gets(local_a0);
	} else {
		do {
			wait(&local_a4);
			if (((local_a4 & 0x7f) == 0) || ('\0' < (char)(((uint8_t)local_a4 & 0x7f) + 1) >> 1)) {
				puts("child is exiting...");
				return 0;
			}
			local_18 = ptrace(PTRACE_PEEKUSER, pid, 44, 0);
		} while (local_18 != 11);
		puts("no exec() for you");
		kill(pid, 9);
	}
	return 0;
}

/* The following functions do nothing */

void enable_timeout_cons(void)
{
	signal(0xe, prog_timeout);
	alarm(0x3c);
	return;
}

void prog_timeout(void)
{
	code *pcVar1;
	
	pcVar1 = (code *)swi(0x80);
	(*pcVar1)();
	return;
}

int get_unum(void)
{
	int local_10 [3];
	
	local_10[0] = 0;
	fflush(stdout);
	__isoc99_scanf(&DAT_08048900,local_10);
	clear_stdin();
	return local_10[0];
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
