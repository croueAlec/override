#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>

/* Decomipled using Ghidra */

typedef struct msg {
	char message[140];
	char username[40];
	int len;
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
	char	message_buffer[1024];

	bzero(message_buffer, 1024);
	puts(">: Msg @Unix-Dude");
	printf(">>: ");
	fgets(message_buffer, 1024, stdin);
	strncpy(message->message, message_buffer, message->len);
	return;
}

void set_username(char *buffer)
{
	char username[128];

	bzero(username, 128);
	puts(">: Enter your username");
	printf(">>: ");
	fgets(username, 128, stdin);
	for (int i = 0; (i < 41 && (username[i] != '\0')); i++) {
		buffer[i] = username[i];
	}
	printf(">: Welcome, %s", buffer);
	return;
}

void handle_msg(void)
{
	msg_t	message;

	bzero(message.username, 40);
	message.len = 140;

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

