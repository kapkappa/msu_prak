// СРАВНИВАЕМ 2 ФАЙЛА И ВЫВОДИМ ПЕРВУЮ СТРОКУ ГДЕ ОНИ РАЗЛИЧАЮТСЯ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char**argv)
{
	if(argc != 3){fprintf(stderr, "I need 2 files\n"); return 1;}
	FILE *f1 = fopen(argv[1], "r");
	FILE *f2 = fopen(argv[2], "r");
	if(f1==NULL || f2==NULL){fprintf(stderr, "Cant read from file\n"); return 1;}
	//
	int size1 = 32, size2 = 32;
	char*str1 = (char*)malloc(size1 * sizeof(char));
	char*str2 = (char*)malloc(size2 * sizeof(char));
	int cmp=0, F;
	while( (F = (fgets(str1, size1-1, f1) == NULL) + (fgets(str2, size2-1, f2) == NULL)) < 1)
	{
		while(str1[strlen(str1)-1] != '\n')
		{
			size1 *= 2;
			str1 = realloc(str1, size1 * sizeof(char));
		}
		while(str2[strlen(str2)-1] != '\n')
		{
			size2 *= 2;
			str2 = realloc(str2, size2 * sizeof(char));
		}
		cmp = strcmp(str1, str2);
	}
	if(F==1) cmp = 1;
	if(cmp == 0)printf("Files are same\n");
	else
	{
		printf("Files are different\n");
		printf("%s",str1);
		printf("%s",str2);
	}
	fclose(f1);
	fclose(f2);
	free(str1);
	free(str2);
	return 0;
}
