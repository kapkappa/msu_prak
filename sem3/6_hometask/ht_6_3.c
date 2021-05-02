#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char**argv)
{
	argc--;
	int i, status;
	pid_t p;
	for(i=1; argc; argc--, i++)
	{
		if((p=fork())==0)
		{
			//SON
			execlp(argv[i], argv[i], NULL);
			perror("Operation's fault");
			exit(1);
		}
		wait(&status);
	}
	return 0;
}
