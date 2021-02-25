// СРАВНИВАЕМ 2 ФАЙЛА И ВЫВОДИМ ПЕРВУЮ СТРОКУ ГДЕ ОНИ РАЗЛИЧАЮТСЯ
// Версия БЕЗ реаллока....

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BLOCKSIZE 4

typedef struct block Block;

struct block{
	char line[BLOCKSIZE];
	Block *next;
};

Block *list = NULL;

void add(Block *block, Block**list)// we adding new block to the end of block's list.
{
	Block *new_block = (Block*)malloc(sizeof(Block));
	if(*list)
	{
		strcpy(new_block->line, block->line);
		new_block->next = (*list)->next;
		(*list)->next = new_block;
		*list = new_block;
	}
	else
	{
		strcpy(new_block->line, block->line);
		new_block->next = new_block;
		*list = new_block;
	}
}

void printL(Block *end)
{
	if(end!=NULL)
	{
		Block *start = end->next;
		while(start != end)
		{
			printf("%s", start->line);
			start=start->next;
		}
		printf("%s", end->line);
	}
}

Block * clear(Block *end)
{
	Block *tmp = NULL;
	if(end!=NULL)
	{
		Block *start = end->next;
		while(start != end)
		{
			tmp=start->next;
			free(start);
			start = tmp;
		}
		free(end);
	}
	return NULL;
}

int main(int argc, char**argv)
{
	if(argc != 3){fprintf(stderr, "I need 2 files\n"); return 1;}
	FILE *f1 = fopen(argv[1], "r");
	FILE *f2 = fopen(argv[2], "r");
	if(f1==NULL || f2==NULL){fprintf(stderr, "Cant read from file\n"); return 1;}

	while(1)
	{
		Block s1, s2;
		if(!fgets(s1.line, sizeof(s1.line), f1))
			s1.line[0]=0;
		if(!fgets(s2.line, sizeof(s2.line), f2))
			s2.line[0]=0;
		if(strcmp(s1.line, s2.line) != 0) // lines are different. need to read the remaining symbols, if they exist, and then print two lines.
		{
			//how to read...
			printf("Files are different!\n");
			printf("Line from the first file: ");
			printL(list);
			printf("%s", s1.line);
			printf("Line from the second file: ");
			printL(list);
			printf("%s", s2.line);
			clear(list);
			break;
		}
		else	//Read to the end? No! just ADD if they are not over!
		{
			if(!s1.line[0]) break;
			if(s1.line[strlen(s1.line)-1] != '\n')
				add(&s1, &list);
			else
				list = clear(list);
		}
	}
	fclose(f1);
	fclose(f2);
	return 0;
}
