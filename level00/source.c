#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Decompiled using Ghidra */

void frame_dummy(void)
{
	return;
}


bool main(void)
{
	int local_14 [4];
	
	puts("***********************************");
	puts("* \t     -Level00 -\t\t  *");
	puts("***********************************");
	printf("Password:");
	__isoc99_scanf(&DAT_08048636,local_14);
	if (local_14[0] != 5276) {
		puts("\nInvalid Password!");
	} else {
		puts("\nAuthenticated!");
		system("/bin/sh");
	}
	return local_14[0] != 5276;
}
