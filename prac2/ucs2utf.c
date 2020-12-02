#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int read_ucs_bom(FILE *fin)
{
	unsigned short bom;
	int order;
	fread(&bom, 2, 1, fin);
	if(bom == 0xFFFE) order = 0;				// Becouse of reverse reading, FFFE read - is FEFF in file. So, its BE order here
	else if(bom == 0xFEFF) order = 1;			// The reason for LE here - is same.
	else
	{
		order = 2;
		fseek(fin, 0, SEEK_SET);
	}
	return order;
}

void read_utf_bom(char *filename)
{
	unsigned char b1, b2, b3;
	b1 = b2 = b3 = 0;
	FILE *fout = fopen(filename, "r");
	if(fout)
	{
		fread(&b1, 1, 1, fout);
		fread(&b2, 1, 1, fout);
		fread(&b3, 1, 1, fout);
		fclose(fout);
	}
	fout = fopen(filename, "w");
	if(!fout)
	{
		perror("Can't rewrite the output file.");
		exit(1);
	}
	if((b1 == 0xEF) && (b2 == 0xBB) && (b3 == 0xBF))
	{
		fwrite(&b1, 1, 1, fout);
		fwrite(&b2, 1, 1, fout);
		fwrite(&b3, 1, 1, fout);
	}
	fclose(fout);
}

long BE_reading(unsigned char*part1, unsigned char*part2, FILE*fin)
{
	long counter = fread(part2, 1, 1, fin);
	counter += fread(part1, 1, 1, fin);
	return counter;
}

long LE_reading(unsigned char*part1, unsigned char*part2, FILE*fin)
{
	long counter = fread(part1, 1, 1, fin);
	counter += fread(part2, 1, 1, fin);
	return counter;
}

int main(int argc, char** argv)
{
	FILE *fin, *fout;
	fin = stdin;
	fout = stdout;
	int byte_order = 2;							//LE - is set default.
	if(argc > 1)
	{
		fin = fopen(argv[1], "r");
		if(!fin)
		{
			perror("Can't read form input file");
			exit(1);
		}
		byte_order = read_ucs_bom(fin);
		if(argc > 2)
		{
			read_utf_bom(argv[2]);
			fout = fopen(argv[2], "a");
			if(!fout)
			{
				perror("Can't write into the output file");
				exit(1);
			}
		}
	}

	unsigned char part1, part2, first, second, third;
	unsigned short symbol;
	long counter = 0;

	long (*reading) (unsigned char*, unsigned char*, FILE*);
	reading = LE_reading;
	if(!byte_order)
	{
		reading = BE_reading;
	}

	while(!feof(fin))
	{
		counter = reading(&part1, &part2, fin);
		if(counter != 2)
		{
			if(counter == 1) fprintf(stderr, "ATTENTION, INCORRECT BYTE'S LENGHT!\n");
			continue;
		}
		symbol = part2;
		symbol <<= 8;
		symbol += part1;
		if(symbol < 0x80)					//We need 1 byte in UTF-8
		{
			fwrite(&part1, 1, 1, fout);
		}
		else if(symbol < 0x800)					//We need 2 bytes: 110[7] + 10[6].
		{
			first = part2 & 0x7;
			first <<= 2;
			first += part1 >> 6;
			first += 0xC0;
			second = 0x80 + (part1 & 0x3F);
			fwrite(&first, 1, 1, fout);
			fwrite(&second, 1, 1, fout);
		}
		else if((symbol < 0xD800) || (symbol > 0xDFFF))		//We need 3 bytes: 1110[4] + 10[6] + 10[6]
		{
			third = 0x80 + (part1 & 0x3F);
			second = part2 & 0xF;
			second <<= 2;
			second += part1 >> 6;
			second += 0x80;
			first = part2 >> 4;
			first += 0xE0;

			fwrite(&first, 1, 1, fout);
			fwrite(&second, 1, 1, fout);
			fwrite(&third, 1, 1, fout);
		}
		else
		{
			fprintf(stderr, "Incorrect symbol\n");
		}
	}
	fclose(fin);
	fclose(fout);
	return 0;
}
