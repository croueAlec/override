#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>


/* Decompiled using Ghidra */

void log_wrapper(FILE *fd, const char *backup_status, const char *filename)
{
	// Canary present

	size_t	newline_index;
	size_t	length;
	char	dest[264];

	strcpy(dest, backup_status);
	length = strlen(dest);
	snprintf(&dest[length], /* 254 - */ length, filename);
	newline_index = strcspn(dest, "\n");
	dest[newline_index] = '\0';
	fprintf(fd, "LOG: %s\n", dest);
	return;
}

uint64_t main(int argc, uint64_t *argv[])
{
	// Canary present
	int				fd;
	FILE			*backup_log_fd;
	FILE			*input_file_fd;
	char			char_buf;
	char			dest[104];
	
	char_buf = -1;
	if (argc != 2) {
		printf("Usage: %s filename\n", *argv);
	}
	backup_log_fd = fopen("./backups/.log","w");
	if (backup_log_fd == 0) {
		printf("ERROR: Failed to open %s\n","./backups/.log");
		exit(1);
	}
	log_wrapper(backup_log_fd, "Starting back up: ", argv[1]);
	input_file_fd = fopen(argv[1], "r");
	if (input_file_fd == 0) {
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}
	strncpy(dest, "./backups/", 11);
	strncat(dest, argv[1], 99 - strlen(dest));
	fd = open(dest, 193, 432);
	if (fd < 0) {
		printf("ERROR: Failed to open %s%s\n","./backups/", argv[1]);
		exit(1);
	}
	do {
		char_buf = fgetc(input_file_fd);
		write(fd, &char_buf, 1);
	} while(char_buf != -1);

	log_wrapper(backup_log_fd, "Finished back up ", argv[1]);
	fclose(input_file_fd);
	close(fd);
	return 0;
}

// 193 011000001
// 432 110110000
