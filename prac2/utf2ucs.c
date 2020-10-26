#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define FIRST_BIT 0x80
#define TWO_BITS 0xC0
#define THREE_BITS 0xE0
#define FOUR_BITS 0xF0

void read_utf_bom(FILE *fin)
{
	unsigned char b1, b2, b3;
	fread(&b1, 1, 1, fin);
	fread(&b2, 1, 1, fin);
	fread(&b3, 1, 1, fin);
	if(!((b1==0xEF) && (b2==0xBB) && (b3==0xBF)))
	{
		fseek(fin, 0, SEEK_SET);
	}
}

int read_ucs_bom(char*filename)
{
	unsigned short bom = 0;
	int order;
	FILE *fout = fopen(filename, "r");
	if(fout)
	{
		fread(&bom, 2, 1, fout);
		fclose(fout);
	}
	if(bom == 0xFEFF) order = 1;			// Becouse of reverse reading, in file is FFFE, thats mean it is LE order.
	else if(bom == 0xFFFE) order = 0;		// Same thing here, in file there are FEFF bytes, which means its BE order.
	else order = 2; 				// No BOM, defoult is LE
	fout = fopen(filename, "w");
	if(!fout){perror("Can't rewrite output file"); exit(1);}
	if(order != 2) fwrite(&bom, 2, 1, fout);	// Write is reversing too, so bom doesnt changes.
	fclose(fout);
	return order;
}

//TODO optimize LE/BE move out from cycle

int main(int argc, char**argv)
{
	FILE *fin, *fout;
	fin = stdin;
	fout = stdout;
	int byte_order = 2;				 // LE is set defoult
	if(argc > 1)
	{
		fin = fopen(argv[1], "r");
		if(!fin){perror("Can't read from the input file"); exit(1);}
		read_utf_bom(fin);			// Skip UTF-BOM
		if(argc>2)
		{
			byte_order = read_ucs_bom(argv[2]);
			fout = fopen(argv[2], "a");
			if(!fout){perror("Can't open or create output file"); exit(1);}
		}
	}
	unsigned char first, second, third;
	unsigned char part1, part2;

	if(!byte_order)					// BE is 0, otherwise its LE.
	{
	}

	while(fread(&first, 1, 1, fin))
	{
		part2 = part1 = 0;
		if(!(first & FIRST_BIT))		// 1 byte 1 symbol
		{
			part1 = first;
			if(!byte_order)
			{
				part2 = part1;
				part1 = 0;
			}
		}
		else if((first & THREE_BITS) == 0xC0)	// first byte from two bytes
		{
			first &= 0x1F;
			fread(&second, 1, 1, fin);
			if((second & TWO_BITS) != 0x80)	//second byte is incorrect!
			{
				fprintf(stderr, "In 2-bytes sequence, the second byte is incorrect!\n");
				fprintf(stderr, "Incorrect byte is %x. And it's position is %ld\n", second, ftell(fin));
				continue;
			}
			second &= 0x3F;

			part1 = first & 0x3;
			part1 <<=6;
			part1 += second;
			part2 = first >> 2;

			if(!byte_order)
			{
				unsigned char tmp = part1;
				part1 = part2;
				part2 = tmp;
			}
		}
		else if((first & FOUR_BITS) == 0xE0) // first byte from three bytes
		{
			first &= 0xF;
			fread(&second, 1, 1, fin);
			if((second & TWO_BITS) != 0x80)
			{
				fprintf(stderr, "In 3-bytes sequence, the second byte is incorrect!\n");
				fprintf(stderr, "Incorrect byte is %x. And it's position is %ld\n", second, ftell(fin));
				continue;
			}
			second &= 0x3F;
			fread(&third, 1, 1, fin);
			if((third & TWO_BITS) != 0x80)
			{
				fprintf(stderr, "In 3-bytes sequence, the third byte is incorrect!\n");
				fprintf(stderr, "Incorrect byte is %x. And it's position is %ld\n", third, ftell(fin));
				continue;
			}
			third &= 0x3F;

			part1 = second & 0x3;
			part1 <<= 6;
			part1 += third;
			part2 = second>>2;
			part2 += first << 4;

			if(!byte_order)
			{
				unsigned short tmp = part1;
				part1 = part2;
				part2 = tmp;
			}
		}
		else
		{
			fprintf(stderr, "The first byte is incorrect\n");
			fprintf(stderr, "Incorrect byte is %x. And it's position is %ld\n", first, ftell(fin));
			continue;
		}
		fwrite(&part1, 1, 1, fout);
		fwrite(&part2, 1, 1, fout);
	}
	fclose(fin);
	fclose(fout);
	return 0;
}
