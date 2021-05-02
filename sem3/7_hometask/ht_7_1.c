#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int counter = 0;
int traps_counter = 0;

void sigtrap_catch(int SIG)
{
	traps_counter++;
//	signal(SIGTRAP, SIG_DFL);
//	kill(getpid(), SIGTRAP);
//	signal(SIGTRAP, catch_sigtrap);
	return;
}

void sigint_counter(int SIG)
{
	signal(SIGINT, sigint_counter);
	counter++;
	printf("\tIt is %d's CNTRL+C (SIGINT),\t and %d SIGTRAPS\n", counter, traps_counter);
	if(counter == 7)
	{
		exit(0);
	}
	else if(counter == 2)
	{
		signal(SIGTRAP, sigtrap_catch);
	}
	else if(counter == 4)
	{
		signal(SIGTRAP, SIG_DFL);
		printf("Between 2th and 4th SIGINT, there were %d SIGTRAPS.\n", traps_counter);
	}
	return;
}

int main(int argc, char**argv)
{
	printf("Enter 7 CNTRL+C to finish\n");
	signal(SIGINT, sigint_counter);
	while(1) pause();
	return 0;
}
