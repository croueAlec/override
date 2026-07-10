#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Decomipled using Ghidra */

void handle_msg(void)
{
	char	buffer[140];

	set_username(buffer);
	set_msg(buffer);
	puts(">: Msg sent!");
	return;
}

int64_t handle_msg()
{
	int64_t	var_3c;
	__builtin_memset(&var_3c, 0, 40);
	int32_t	var_14 = 140;
	char	var_c8[140];

	set_username(&var_c8);
	set_msg(&var_c8);
	return puts(">: Msg sent!");
}

void secret_backdoor(void)
{
	char	command[128];

	fgets(command, 128, stdin);
	system(command);
	return;
}

void set_msg(char *buffer)
{
	char	message[1024];

	bzero(message, 1024);
	puts(">: Msg @Unix-Dude");
	printf(">>: ");
	fgets(message, 1024, stdin);
	strncpy(buffer, message, (long)*(int *)(buffer + 180));
	return;
}

void set_username(char *buffer)
{
	char username[140];

	bzero(username, 128);
	puts(">: Enter your username");
	printf(">>: ");
	fgets(username, 128, stdin);
	for (int i = 0; (i < 41 && (username[i] != '\0')); i++) {
		buffer[140 + i] = username[i];
	}
	printf(">: Welcome, %s", buffer + 140);
	return;
}

uint64_t main(void)
{
	puts(
			"--------------------------------------------\n \
			|   ~Welcome to l33t-m$n ~    v1337          | \
			\n--------------------------------------------"
			);
	handle_msg();
	return 0;
}
