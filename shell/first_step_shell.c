#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define RESET "\e[m"
#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define BLUE "\e[1;34m"
#define MAGENTA "\e[1;35m"
#define CYAN "\e[1;36m"
#define COLOURS 6

typedef struct node
{
	struct node *next;
	char *word;
}node;

short eoflag = 0;
short Qflag = 0;
short Newlineflag = 0;
char*colour[COLOURS] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

char*get_random_colour()
{
	srand(time(NULL));
	int num = rand() % COLOURS;
	return colour[num];
}

char**list_to_mas(node*list)
{
	char**argv = (char**)malloc(sizeof(char*));
	int i = 0;
	while(list)
	{
		argv[i++] = list->word;
		argv = realloc(argv, sizeof(char*) * (i+1));
		list=list->next;
	}
	argv[i] = NULL;
	return argv;
}

node*insert(node*list, char*word)
{
	if(list==NULL)
	{
		list = (node*)malloc(sizeof(node));
		list->next = NULL;
		list->word = word;
		return list;
	}
	list->next = insert(list->next, word);
	return list;
}

void print(node*list)
{
	if(list)
	{
		printf("%s\n",list->word);
		print(list->next);
	}
}

void delet(node *list)
{
	node *tmp;
	while(list)
	{
		tmp=list->next;
		free(list->word);
		free(list);
		list=tmp;
	}
}

char*readword()
{
	int size = 16, indx = 0;
	char*word = (char*)malloc(sizeof(char) * size);
	char c=0;
	while((c=getchar())!= EOF)
	{
		if(Qflag)
		{
			if(c=='"') Qflag = 0;
			else
			{
				indx++;
				if(indx == size)
				{
					size *= 2;
					word = realloc(word, sizeof(char) * size);
				}
				word[indx-1] = c;
			}
		}
		else if(isspace(c))
		{
			if(c=='\n') Newlineflag = 1;
			word[indx] = 0;
			return word;
		}
		else if(c=='"')
		{
			Qflag = 1;
		}
		else
		{
			indx++;
			if(indx==size)
			{
				size*=2;
				word = realloc(word, sizeof(char) * size);
				if(!word)
				{
					perror("Can't realloc");
					exit(1);
				}
			}
			word[indx-1] = c;
		}
	}
	word[indx] = 0;
	eoflag = 1;
	return word;
}

void check_exit(node*list)
{
	if(!strcmp(list->word, "exit"))
	{
		delet(list);
		printf("Exit\n");
		exit(0);
	}
}

int check_cd(char**argv)
{
	if(!strcmp(argv[0], "cd"))
	{
		if(argv[1]) chdir(argv[1]);
		else chdir(getenv("HOME"));
		return 1;
	}
	else return 0;
}

int run(node*list)
{
	check_exit(list);
	char**argv = list_to_mas(list);
	if(check_cd(argv))
	{
		free(argv);
		return 0;
	}

	pid_t p;
	int status;
	if((p=fork())==0)
	{
		execvp(argv[0], argv);
		perror("Command error");
		exit(1);
	}
	wait(&status);
	free(argv);
	return 0;
}

int main(int argc, char **argv)
{
	char*w = NULL;
	char*col = get_random_colour();
	node*list = NULL;
	while(!eoflag)
	{
		printf("%s> %s", col, RESET);
		Newlineflag = 0;
		list = NULL;
		while(!eoflag && !Newlineflag)
		{
			w = readword();
			if(w[0]!=0)	list = insert(list, w);
		}
		if(!eoflag && list)
		{
			run(list);
		}
//		if(list) print(list);
		delet(list);
	}
	printf("\n%s Bye! %s\n", col, RESET);
	return 0;
}
