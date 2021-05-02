#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* my_strcat(char *des, char *dep)
{
	char*tmp=des;
	while(*des++);
	des--;
	while((*des++ = *dep++));
	return tmp;
}

int strend(char*s, char*subs)
{
	int slen = strlen(s);
	int subslen = strlen(subs);
	if(subslen > slen) return 0;
	int i;
	for(i=subslen; *(s+slen) == *(subs+i);)
	{
		slen--;
		i--;
		if(i==-1) break;
	}
	return i<0;
}

void my_strncpy(char*des, char*dep, int n)
{
	while(*dep && n--)
		*des++ = *dep++;
	printf("DEBUG\n");
	while(n-- > 0)
		*des++ = 0;
}

void my_strncat(char*des, char*dep, int n)
{
	while(*des++);
	des--;
	while(n-- && (*des++ = *dep++));
	*des=0;
}

int my_strncmp(char*str1, char*str2, int n)
{
	for(; *str1 == *str2; str1++, str2++)
	{
		if(*str1==0 || --n<=0)
			return 0;
	}
	return *str1-*str2;
}

int main()
{
	int size = 16;
	char*str1 = malloc(size*sizeof(char));
	char*str2 = malloc(size*sizeof(char));
	fgets(str1,size,stdin);
	fgets(str2,size,stdin);
//	printf("my_strncmp: %d\n", my_strncmp(str1, str2, 5));
//	my_strncat(str1, str2, 5); printf("my_strncat: %s\n", str1);
//	my_strncpy(str1, str2, 5); printf("my_strncpy: %s\n", str1);
//	printf("strend: %d\n", strend(str1, str2));
	printf("my_strcat: %s", my_strcat(str1, str2));
	free(str1);
	free(str2);
	return 0;
}
