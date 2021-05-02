#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define ARGS_ERR "Incorrect number of arguments\n"

void item_a(int argc, char**argv)
{
	if(argc!=5)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	int fd[2];
	int fread = dup(0);
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[1], 1);				//Now STDOUT writes in pipe
		close(fd[1]);
		close(fd[0]);
		execlp(argv[2], argv[2], NULL);
		perror("Item a, first exec");
		exit(1);
	}
	dup2(fd[0], fread);
	close(fd[0]);
	close(fd[1]);
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fread, 0);				//now STDIN is pipe's hole
		close(fread);
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
		execlp(argv[3], argv[3], NULL);
		perror("Item a, second exec");
		exit(1);
	}
	dup2(fd[0], fread);
	close(fd[0]);
	close(fd[1]);
	if((p=fork())==0)
	{
		dup2(fread, 0);
		close(fread);
		execlp(argv[4], argv[4], NULL);
		perror("Item a, third exec");
		exit(1);
	}
	close(fread);
	while(wait(NULL)!=-1);
}

void item_b(int argc, char**argv)
{
	if(argc!=5)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	int fd[2];
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		execlp(argv[2], argv[2], NULL);
		perror("Item b, first exec");
		exit(1);
	}
	if((p=fork())==0)
	{
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		int f = open(argv[4], O_WRONLY | O_TRUNC | O_CREAT, 0664);
		if(f==-1)
		{
			perror("Item b, output file");
			exit(1);
		}
		dup2(f, 1);
		close(f);
		execlp(argv[3], argv[3], NULL);
		perror("Item b, second exec");
		exit(1);
	}
	close(fd[0]);
	close(fd[1]);
	while(wait(NULL)!=-1);
}

void item_c(int argc, char**argv)
{
	if(argc!=9)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	int fd[2];
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		int f = open(argv[5], O_RDONLY);
		if(f==-1)
		{
			perror("Item c, output file");
			exit(1);
		}
		dup2(f, 0);
		close(f);
		execlp(argv[2], argv[2], argv[3], argv[4], NULL);
		perror("Item c, first exec");
		exit(1);
	}
	if((p=fork())==0)
	{
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		execlp(argv[6], argv[6], argv[7], argv[8], NULL);
		perror("Item c, third exec");
		exit(1);
	}
	close(fd[0]);
	close(fd[1]);
	while(wait(NULL)!=-1);
}

void item_d(int argc, char**argv)
{
	if(argc!=5)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	if((p=fork())==0)
	{
		int fr = open(argv[3], O_RDONLY);
		int fw = open(argv[4], O_WRONLY | O_TRUNC | O_CREAT, 0664);
		if(fr==-1 || fw==-1)
		{
			perror("Item d, files");
			exit(1);
		}
		dup2(fr, 0);
		dup2(fw, 1);
		close(fr);
		close(fw);
		execlp(argv[2], argv[2], NULL);
		perror("Item d, exec");
		exit(1);
	}
	wait(NULL);
}

void item_e(int argc, char**argv)
{
	if(argc!=7)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	int fd[2];
	int fread = dup(0);
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		int f = open(argv[3], O_RDONLY);
		if(f==-1)
		{
			perror("Item e, input file");
			exit(1);
		}
		dup2(f, 0);
		close(f);
		execlp(argv[2], argv[2], NULL);
		perror("Item d, first exec");
		exit(1);
	}
	dup2(fd[0], fread);
	close(fd[0]);
	close(fd[1]);
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		dup2(fread, 0);
		close(fread);
		execlp(argv[4], argv[4], NULL);
		perror("Item e, second exec");
		exit(1);
	}
	dup2(fd[0], fread);
	close(fd[0]);
	close(fd[1]);
	if((p=fork())==0)
	{
		dup2(fread, 0);
		close(fread);
		int f = open(argv[6], O_WRONLY | O_TRUNC | O_CREAT, 0664);
		if(f==-1)
		{
			perror("Item e, output file");
			exit(1);
		}
		dup2(f, 1);
		close(f);
		execlp(argv[5], argv[5], NULL);
		perror("Item e, third exec");
		exit(1);
	}
	close(fread);
	while(wait(NULL)!=-1);
}

void item_f(int argc, char**argv)
{
	if(argc!=5)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	int fd[2];
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
		execlp(argv[2], argv[2], NULL);
		perror("Item f, first exec");
		exit(1);
	}
	if((p=fork())==0)
	{
		int f = open(argv[4], O_WRONLY | O_APPEND | O_CREAT, 0664);
		if(f==-1)
		{
			perror("Item f, output file");
			exit(1);
		}
		dup2(f, 1);
		close(f);
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		execlp(argv[3], argv[3], NULL);
		perror("Item f, second exec");
		exit(1);
	}
	close(fd[0]);
	close(fd[1]);
	while(wait(NULL)!=-1);
}

void item_g(int argc, char**argv)
{
	if(argc!=6)
	{
		fprintf(stderr, ARGS_ERR);
		exit(1);
	}
	pid_t p;
	if((p=fork())==0)
	{
		execlp(argv[2], argv[2], NULL);
		perror("Item g, first exec");
		exit(1);
	}
	wait(NULL);
	int fd[2];
	pipe(fd);
	if((p=fork())==0)
	{
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		execlp(argv[3], argv[3], NULL);
		perror("Item g, second exec");
		exit(1);
	}
	if((p=fork())==0)
	{
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
		int f = open(argv[5], O_WRONLY | O_TRUNC | O_CREAT, 0664);
		if(f==-1)
		{
			perror("Item g, output file");
			exit(1);
		}
		dup2(f, 1);
		close(f);
		execlp(argv[4], argv[4], NULL);
		perror("Item g, third exec");
		exit(1);
	}
	close(fd[0]);
	close(fd[1]);
	while(wait(NULL)!=-1);
}

int main(int argc, char**argv)
{
	char*c=NULL;
	const char*items = "abcdefg";
	if(argc==1)
	{
		fprintf(stderr, "No args!\n");
		return 1;
	}
	else if(strlen(argv[1])!=1)
	{
		fprintf(stderr, "Need a single letter as an item indication\n");
		return 1;
	}
	else
	{
		c = strchr(items, argv[1][0]);
		if(!c)
		{
			fprintf(stderr, "There is no such item\n");
			return 1;
		}
	}
	switch(*c)
	{
		case 'a':
			printf("Item a\n");
			item_a(argc, argv);
			break;
		case 'b':
			printf("Item b\n");
			item_b(argc, argv);
			break;
		case 'c':
			printf("Item c\n");
			item_c(argc, argv);
			break;
		case 'd':
			printf("Item d\n");
			item_d(argc, argv);
			break;
		case 'e':
			printf("Item e\n");
			item_e(argc, argv);
			break;
		case 'f':
			printf("Item f\n");
			item_f(argc, argv);
			break;
		case 'g':
			printf("Item g\n");
			item_g(argc, argv);
			break;
		default:
			fprintf(stderr, "No item indication\n");
	}
	return 0;
}
