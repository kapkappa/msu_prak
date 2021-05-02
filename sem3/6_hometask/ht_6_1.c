#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char**argv)
{
	if(argc!=3)
	{
		fprintf(stderr, "Need operations names\n");
		return 1;
	}
	pid_t p;
	int status=0;
	if((p = fork()) == 0)
	{
		// SON
		execlp(argv[1], argv[1], NULL);
		perror("First operation's fault");
		exit(1);
	}
	wait(&status);
	if(WIFEXITED(status) && WEXITSTATUS(status)==0)
	{
		return 0;
	}
	if((p = fork()) == 0)
	{
		execlp(argv[2], argv[2], NULL);
		perror("Second operation's fault");
		exit(1);
	}
	wait(&status);
	return 0;
}
