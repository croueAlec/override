#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>

/* Decomipled using Ghidra */

typedef struct msg {
	char message[140];
	char username[40];
	int length;
} msg_t;


void secret_backdoor(void)
{
	char	command[128];

	fgets(command, 128, stdin);
	system(command);
	return;
}

void set_msg(msg_t *message)
{
	char	buffer[1024];

	bzero(buffer, 1024);
	puts(">: Msg @Unix-Dude");
	printf(">>: ");
	fgets(buffer, 1024, stdin);
	strncpy(message->message, buffer, message->length);
	return;
}

void set_username(char *username)
{
	char buffer[128];

	bzero(buffer, 128);
	puts(">: Enter your username");
	printf(">>: ");
	fgets(buffer, 128, stdin);
	for (int i = 0; (i < 41 && (buffer[i] != '\0')); i++) {
		username[i] = buffer[i];
	}
	printf(">: Welcome, %s", username);
	return;
}

void handle_msg(void)
{
	msg_t	message;

	bzero(message.username, 40);
	message.length = 140;

	set_username(message.username);
	set_msg(message.message);
	puts(">: Msg sent!");
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

