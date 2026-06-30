#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

/* Decompiled using Ghidra */

char	a_user_name[257] = {0};

int verify_user_name(void)
{
	uint8_t uVar4;
	uint8_t uVar5;
	
	uVar4 = &stack0xfffffff4 < (uint8_t *)16;
	uVar5 = &stack0x00000000 == (uint8_t *)28;
	puts("verifying username....\n");
	strcmp(a_user_name, "dat_wil");
	return (int)(char)((!(bool)uVar4 && !(bool)uVar5) - uVar4);
}

int verify_user_pass(uint8_t *param_1)
{
	int iVar1;
	uint8_t *pbVar2;
	uint8_t in_CF;
	uint8_t in_ZF;
	
	iVar1 = 5;
	pbVar2 = (uint8_t *)"admin";
	do {
		if (iVar1 == 0)
			break;
		iVar1 = iVar1 + -1;
		in_CF = *param_1 < *pbVar2;
		in_ZF = *param_1 == *pbVar2;
		param_1 = param_1 + 1;
		pbVar2 = pbVar2 + 1;
	} while ((bool)in_ZF);
	return (int)(char)((!(bool)in_CF && !(bool)in_ZF) - in_CF);
}

int main(void)
{
	char password_buffer [64];
	int local_14;

	bzero(password_buffer, 64);

	local_14 = 0;
	puts("********* ADMIN LOGIN PROMPT *********");
	printf("Enter Username: ");
	fgets(&a_user_name, 256, stdin);
	local_14 = verify_user_name();
	if (local_14 == 0) {
		puts("Enter Password: ");
		fgets(password_buffer, 100, stdin);
		local_14 = verify_user_pass(password_buffer);
		if ((local_14 == 0) || (local_14 != 0)) {
			puts("nope, incorrect password...\n");
			return 1;
		} else {
			return 0;
		}
	}
	else {
		puts("nope, incorrect username...\n");
		return 1;
	}
}
