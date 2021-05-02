#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int max_count = 1;
int count = 1;

void My_signal(int SIG)
{
	printf("I got SIGINT %d times.\n", count++);
	if(count >= max_count) signal(SIGINT, SIG_DFL);
	else signal(SIGINT, My_signal);
}

int main(int argc, char**argv)
{
	if(argc!=1) max_count = atoi(argv[1]);
	if(max_count>1 && max_count < 20) signal(SIGINT, My_signal);
	else if (max_count != 1)
	{
		printf("Invalid number of max_count.\n");
		return 1;
	}
	printf("Need %d SIGINTs.\n", max_count);
	while(1)
	{
		pause();
	}
	return 0;
}
