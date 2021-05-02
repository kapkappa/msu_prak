#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int counter = 0;
pid_t pid1 = 0, pid2 = 0;
int fd[2];

void throw(int SIG)
{
	read(fd[0], &counter, sizeof(int));
	counter++;
	if(pid1 && pid2)
	{
		//	Father
		printf("Father (pid=%d), counter = %d\n", getpid(), counter);
		write(fd[1], &counter, sizeof(int));
		sleep(1);
		kill(pid2, SIGUSR1);
	}
	else if(pid1)
	{
		//	Son 2
		printf("Son 2 (pid=%d), counter = %d\n", getpid(), counter);
		write(fd[1], &counter, sizeof(int));
		kill(pid1, SIGUSR1);
	}
	else
	{
		//	Son 1
		printf("Son 1 (pid=%d), counter = %d\n", getpid(), counter);
		write(fd[1], &counter, sizeof(int));
		kill(getppid(), SIGUSR1);
	}
	return;
}

void end(int SIG)
{
	if(!(pid1 && pid2))
	{
		close(fd[0]);
		close(fd[1]);
		exit(0);
	}
	return;
}

int main()
{
	signal(SIGUSR1, throw);
	signal(SIGINT, 	end);
	pipe(fd);
	printf("Father (pid=%d)\n", getpid());
	if(!(pid1 = fork()))
	{
		//	Son 1
		while(1) pause();
	}
	printf("Son 1 (pid=%d)\n", pid1);
	if(!(pid2 = fork()))
	{
		//	Son 2
		while(1) pause();
	}
	printf("Son 2 (pid=%d)\n", pid2);
	printf("START\n");
	write(fd[1], &counter, sizeof(int));
	kill(pid2, SIGUSR1);
	wait(NULL);
	wait(NULL);
	close(fd[1]);
	close(fd[0]);
	printf("END\n");
	return 0;
}
