#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <stdbool.h>

/* Decompiled using Ghidra */

int main(int param_1,int *param_2,int *param_3)
{
	int data[100];						// < Where numbers are stored
	int failure = false;
	uint8_t command_buffer [20];

	bzero(command_buffer, 20);
	bzero(data, 100);

	puts(
		"----------------------------------------------------	\n \
		Welcome to wil\'s crappy number storage service!		\n \
		----------------------------------------------------	\n \
		Commands:												\n \
		  store - store a number into the data storage			\n \
		  read	- read a number from the data storage			\n \
		  quit	- exit the program								\n \
		----------------------------------------------------	\n \
		  wil has reserved some storage :>						\n \
		----------------------------------------------------	\n"
		);
	do {
		printf("Input command: ");
		failure = 1;
		fgets(command_buffer, 20, stdin);

		if (memcmp(command_buffer, "store", 5) == 0) {
			failure = store_number(data);
		}
		else if (memcmp(command_buffer, "read", 4) == 0) {
			failure = read_number(data);
		} else if (memcmp(command_buffer, "quit", 4) == 0) {
				return;
		}

		if (failure == true) {
			printf(" Failed to do %s command\n",command_buffer);
		} else {
			printf(" Completed %s command successfully\n",command_buffer);
		}

		bzero(command_buffer, 19);
	} while( true );
}


int read_number(int *data)
{
	int index;
	
	printf(" Index: ");
	index = get_unum();
	printf(" Number at data[%u] is %u\n", index, data[index]);
	return 0;
}


int store_number(int *data)
{
	size_t	number;
	size_t	index;

	printf(" Number: ");
	number = get_unum();

	printf(" Index: ");
	index = get_unum();

	if ((index % 3 == 0) || (number >> 24 == 183)) {		// 0d3070230528 == 0xB7000000
		puts(" *** ERROR! ***");
		puts("	 This index is reserved for wil!");
		puts(" *** ERROR! ***");
		return 1;
	} else {
		data[index] = number;	// *(uint *)(index * 4 + data) = number;	< what the program actually does
		return 0;
	}
}

size_t get_unum(void)
{
	size_t	result = 0;

	fflush(stdout);
	scanf("%u", &result);
	clear_stdin();

	return result;
}

void clear_stdin(void)
{
	char	c;

	do {
		c = getchar();
		if (c == '\n') {
			return;
		}
	} while (c != -1);

	return;
}

/* Omitted functions */

prog_timeout();

frame_dummy();
