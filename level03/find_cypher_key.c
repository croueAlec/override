#include <stdio.h>

int main(int argc, char const *argv[])
{
	
	for (size_t i = 1; i < 22; i++)
	{
		char	str[] = "Q}|u`sfg~sf{}|a3";
		for (size_t j = 0; str[j] != '\0'; j++)
		{
			str[j] ^= i;
		}
		printf("%zu : %s\n", i, str);
	}

	return 0;
}
