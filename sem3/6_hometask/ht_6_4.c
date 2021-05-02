#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char**argv)
{
	int i, status;
	int nice = 0, half_nice = 0, not_nice = 0;
	pid_t p[argc-1];
	for(i=1;i<argc;i++)
	{
		if((p[i-1]=fork())==0)
		{
			execlp(argv[i], argv[i], NULL);
			perror("process exec");
			exit(1);
		}
	}
	for(i=1;i<argc;i++)
	{
		wait(&status);
		if(WIFEXITED(status) && !WEXITSTATUS(status))
		{
			nice++;
		}
		else if(WIFEXITED(status))
		{
			half_nice++;
		}
		else
		{
			not_nice++;
		}
	}
	printf("Total processes: %d\n", argc-1);
	printf("Normal completion (for programmer): %d\n", nice);
	printf("Normal completion (for OS): %d\n", half_nice);
	printf("Abnormal completion: %d\n", not_nice);
	return 0;
}
