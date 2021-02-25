//CLASSWORK FROM 15-09-2020
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char*argv[])
{
	int size = 10, i=0, max_len=0, c=0;

	char *max_str = (char*)malloc(size);
	char *current_str = (char*)malloc(size);

	while((c=getchar()) != EOF)
	{
		if((i+1) == size)
		{
			size*=2;
			current_str = realloc(current_str,size);
		}
		current_str[i]=c;
		i++;
		if(c=='\n')
		{
			current_str[i]=0;
			if(i > max_len)
			{
				//current to max
				max_str = realloc(max_str, i+1);
				max_str = strcpy(max_str, current_str);
				max_len = i;
			}
			i=0;
			free(current_str);
			current_str = (char*)malloc(size);
		}
	}

	printf("%s", max_str);
	free(max_str);
	free(current_str);
	if(current_str == NULL) printf("null\n");
	return 0;
}
