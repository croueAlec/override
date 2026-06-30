#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char* ptr = getenv(argv[1]);
	printf("%p\n", ptr);

	return (0);
}
