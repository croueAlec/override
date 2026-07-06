#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

/* Decompiled using Ghidra */

uint64_t main(void)
{
	size_t	newline_index;
	char	password [112];
	char	read_buffer [48];
	char	username [100];
	int		length_read;
	FILE	*password_fd;
	
	bzero(username, 100);
	bzero(read_buffer, 41);
	bzero(password, 100);
	password_fd = (FILE *)0x0;
	length_read = 0;
	password_fd = fopen("/home/users/level03/.pass", "r");
	if (password_fd == (FILE *)0x0) {
		fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
										/* WARNING: Subroutine does not return */
		exit(1);
	}
	length_read = fread(read_buffer, 1, 41, password_fd);
	newline_index = strcspn(read_buffer,"\n");
	read_buffer[newline_index] = '\0';
	if (length_read != 41) {
		fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
		fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
										/* WARNING: Subroutine does not return */
		exit(1);
	}
	fclose(password_fd);

	puts("===== [ Secure Access System v1.0 ] =====");
	puts("/***************************************\\");
	puts("| You must login to access this system. |");
	puts("\\**************************************/");

	printf("--[ Username: ");
	fgets(username, 100, stdin);
	newline_index = strcspn(username, "\n");
	username[newline_index] = '\0';
	printf("--[ Password: ");
	fgets(password, 100, stdin);
	newline_index = strcspn(password, "\n");
	password[newline_index] = '\0';
	puts("*****************************************");

	if (strncmp(read_buffer, password, 41) == 0) {
		printf("Greetings, %s!\n", username);
		system("/bin/sh");
		return 0;
	}
	printf(username);
	puts(" does not have access!");
										/* WARNING: Subroutine does not return */
	exit(1);
}
