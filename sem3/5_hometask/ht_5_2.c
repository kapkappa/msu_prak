#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

/*
	in argv - binary file, that contain integer values.
	Task - Sort this file, without any extra tools like dynamic memory
*/

int read_number(int fd, char*number, int*size)
{
	char c=0;
	int counter=0, i=1;
	while(!isdigit(c) && c!='-')
	{
		counter = read(fd, &c, sizeof(char));
		if(!counter) return 0;
	}
	*number = c;
	while(counter)
	{
		counter = read(fd, &c, sizeof(char));
		if(counter && isdigit(c))
		{
			if(++i == *size)
			{
				*size *= 2;
				number = realloc(number, sizeof(char) * *size);
			}
			number[i-1] = c;
		}
		else break;
	}
	number[i]=0;
	return counter;
}

int main(int argc, char**argv)
{
	if(argc == 1)
	{
		fprintf(stderr, "Need filename\n");
		return 1;
	}
	int fd = open(argv[1], O_RDWR);
	int count = 1, size=16;
	char*num1 = (char*)malloc(sizeof(char) * size);
	char*num2 = (char*)malloc(sizeof(char) * size);
	while(count)
	{
		count = read_number(fd, num1, &size);
//		printf("number1 is %s\n", num1);
		count = read_number(fd, num2, &size);
//		printf("number2 is %s\n", num2);
		if(!count) break;
		if((atoi(num1) <= atoi(num2)))
		{
			int l = strlen(num2)+1;
			lseek(fd, -l, SEEK_CUR);
		}
		else
		{
//			printf("Changing\n");
			int l = strlen(num1) + strlen(num2) + 2;
//			printf("l is %d\n", l);
			lseek(fd, -l, SEEK_CUR);
			char space = ' ';
			write(fd, num2, sizeof(char) * strlen(num2) );
			write(fd, &space, sizeof(char));
			write(fd, num1, sizeof(char) * strlen(num1) );
			write(fd, &space, sizeof(char));
			lseek(fd, 0, SEEK_SET);
		}
	}
	close(fd);
	return 0;
}
