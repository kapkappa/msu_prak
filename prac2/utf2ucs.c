#include <stdio.h>
#include <stdlib.h>

/*
	Read from stdin, write in stdout
	I want to read some bytes in UTF-8, this means:
	there are 3 types of encoded symbols - 1, 2 and 3 bytes for symbol

	0[7] - 1 byte for syvbol,
	110[5] AND 10[6] - 2 bytes for symbol,
	1110[4] AND 10[6] AND 10[6] - 3 bytes for symbol,

	So we need to check first bits in read byte. And then decide how it will be decoded into UTF-16

	In UCS one symbol is encoded with 2 bytes OR with 2 pairs of bytes
*/

#define FIRST_BIT 0200

int main(int argc, char**argv)
{

//in case that we need to read symbols byte after byte then we can use any read function
	FILE *fin, *fout;
	fin = stdin;
	fout = stdout;
	char LE = 1; // We have preordered numeration - LESS ORDER!
	char first, second, third, fbit;
	unsigned short ucs_byte;
	fread(&first, sizeof(char), 1, fin);
	fread(&second, sizeof(char), 1, fin);
	if((first == 0xFF) && (second == 0xFE))
	{
		LE = 1;
	}
	else if((first == 0xFE) && (second == 0xFF))
	{
		LE = 0; // WE HAVE BIG ENDIAN
	}
	else
	{
		LE = 1;
		ungetc(second, fin);
		ungetc(first, fin);
	}

	while(!feof(fin))
	{
		fread(&first, sizeof(char), 1, fin);
		// Проверить первые биты
		fbit = first & FIRST_BIT;
		if(!fbit)
		{
			// 1 BYTE 1 SYMBOL
			ucs_byte = 0;
			ucs_byte = ucs_byte | first;
			fwrite(&ucs_byte, sizeof(ucs_byte), 1, fout);
		}
	}
}
