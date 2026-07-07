#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Decompiled using Hex-Rays (not Ghidra this time) */

void	main(void)
{
	char	s[100];			// 0x64
	size_t	i;

	i = 0;
	fgets(s, 100, stdin);
	for ( i = 0; i < strlen(s); ++i )
	{
		if ( s[i] >= 'A' && s[i] <= 'Z' )
			s[i] ^= 32;						// Converts to uppercase letters to lowercase
	}

	printf(s);
	exit(0);
}
