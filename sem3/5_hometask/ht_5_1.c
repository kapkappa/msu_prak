#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 10

char* myfgets(int fd, char*pBuffer, size_t size)
{
	size_t cnt = size--;
	char* head=pBuffer;
	ssize_t indx=1;
	while(indx && --cnt)
	{
		indx = read(fd, pBuffer, 1);
		if(indx < 0) return NULL;
		if(indx && *pBuffer++ == '\n')	break;
	}
	if(!indx && (cnt==size)) return NULL;
	*pBuffer = 0;
	return head;
}

void read_from_cmd(char*string)
{
	int fd = 0;
	int cnt = 1;
	while((string = myfgets(fd, string, SIZE))) printf("iter №%d:\n%s", cnt++, string);
}

void read_from_file(char*string, char*filename)
{
	int fd = open(filename, O_RDONLY);
	int cnt = 1;
	while((string = myfgets(fd, string, SIZE))) printf("iter №%d:\n%s", cnt++, string);
	close(fd);
}


int main(int argc, char**argv)
{
	char*string=(char*)malloc(sizeof(char) * SIZE);
	if(argc == 1)
	{
		read_from_cmd(string);
	}
	else
	{
		read_from_file(string, argv[1]);
	}
	free(string);
	return 0;
}
