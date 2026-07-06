#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/evp.h>

/* Decompiled using Ghidra */

int	decrypt(int result)
{
	size_t	len;
	char	cypher[] = "Q}|u`sfg~sf{}|a3";
	
	len = strlen(cypher);
	for (size_t i = 0; i < len; i++)
	{
		cypher[i] ^= result;
	}
	
	if (strcmp(cypher, "Congratulations!") == 0) {
		return system("/bin/sh");
	} else {
		return puts("\nInvalid Password");
	}
}

void	test(int user_input, int constant)
{
	uint32_t	result = constant - user_input;
	
	switch (result) {
		default:
		result = rand();
		decrypt(result);
		break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 16:
		case 17:
		case 18:	// < We need result to have a value of 18
		case 19:
		case 20:
		case 21:
		decrypt(result);
	}
	return;
}

int main(void)
{
	time_t	random_seed;
	int		user_input;
	
	random_seed = time((time_t *)0x0);
	srand(random_seed);
	puts("***********************************");
	puts("*\t\tlevel03\t\t**");
	puts("***********************************");
	printf("Password:");
	scanf("%d", &user_input);
	test(user_input, 322424845);
	return 0;
}

/* ==== The functions below are not used ==== */

void	clear_stdin(void)
{
	int	iVar1;

	do {
		iVar1 = getchar();
		if ((char)iVar1 == '\n') {
			return;
		}
	} while ((char)iVar1 != -1);
	return;
}


int	get_unum(void)
{
	int local_10 [3];
	
	local_10[0] = 0;
	fflush(stdout);
	scanf(&DAT_080489c0,local_10);
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
