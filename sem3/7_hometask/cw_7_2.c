#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int count = 0;
int max_count = 10;
pid_t target_pid;
int fd[2];

void pingpong(int SIG)
{
	signal(SIGUSR1, pingpong);
	if(count < max_count)
	{
		read(fd[0], &count, sizeof(count));
		printf("Count is %d\n", count);
		count++;
		write(fd[1], &count, sizeof(count));
		kill(target_pid, SIGUSR1);
	}
	else if(!target_pid)
	{
		//we in Son
		close(fd[0]);
		close(fd[1]);
		exit(0);
	}
	//we in Father
	kill(target_pid, SIGUSR1);
}

int main(int argc, char**argv)
{
	if(argc > 1 && (max_count = atoi(argv[1])) > 0)
	{
		printf("Max count sets to %d.\n", max_count);
	}
	else
	{
		max_count = 10;
		printf("Max count sets as default, equal 10.\n");
	}
	signal(SIGUSR1, pingpong);
	pipe(fd);
	if((target_pid = fork()))
	{
		//Father
		write(fd[1], &count, sizeof(int));
		kill(target_pid, SIGUSR1);
		wait(NULL);
	}
	else
	{
		//SON
		while(1) pause();
	}
	close(fd[0]);
	close(fd[1]);
	return 0;
}

