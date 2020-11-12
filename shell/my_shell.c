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

typedef struct tree
{
	node *argv;
	struct tree *left;
	struct tree *right;
	int Wr;
	int Rd;
}tree;

short eoflag = 0;
short Qflag = 0;
short Newlineflag = 0;
short Specflag = 0;
short Pipeflag = 0;

char*colour[COLOURS] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

char*dup_spec_symbols = ">|&";
char*one_spec_symbols = "<;";

char*get_random_colour()
{
	srand(time(NULL));
	int num = rand() % COLOURS;
	return colour[num];
}

char**list_to_mas(node*list)
{
	char**result = (char**)malloc(sizeof(char*));
	int i = 0;
	while(list)
	{
		result[i++] = list->word;
		result = realloc(result, sizeof(char*) * (i+1));
		list=list->next;
	}
	result[i] = NULL;
	return result;
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

void print_list(node*list)
{
	if(list)
	{
		printf("%s  ",list->word);
		print_list(list->next);
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

void delet_list(node *list)
{
	if(list)
	{
		delet_list(list->next);
		free(list);
	}
}

char*readword()
{
	int size = 16, indx = 0;
	Specflag = 0;
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
		else if(strchr(one_spec_symbols, c))
		{
			if(indx)
			{
				ungetc(c, stdin);
				word[indx]=0;
				return word;
			}
			else
			{
				word[0] = c;
				word[1] = 0;
				return word;
			}
		}
		else if(strchr(dup_spec_symbols, c))
		{
			if(Specflag)
			{
				if(word[0] == c)
				{
					word[1] = c;
					word[2] = 0;
				}
				else
				{
					ungetc(c, stdin);
					word[1] = 0;
				}
				return word;
			}
			else if(indx)
			{
				ungetc(c, stdin);
				word[indx] = 0;
				return word;
			}
			//First symbol is special
			Specflag = 1;
			word[indx++] = c;
		}
		else if(Specflag)
		{
			ungetc(c, stdin);
			word[1] = 0;
			return word;
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
		printf("\n Exit \n");
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

int do_list(node*list)
{
	check_exit(list);
	char**args = list_to_mas(list);
	if(check_cd(args))
	{
		free(args);
		return 0;
	}

	pid_t p;
	int status;
	if((p=fork())==0)
	{
		execvp(args[0], args);
		perror("Command error");
		exit(1);
	}
	wait(&status);
	free(args);
	return 0;
}

tree*create_node(char*word)
{
	tree*res=(tree*)malloc(sizeof(tree));
	res->right = NULL;
	res->left = NULL;
	res->argv = NULL;
	res->argv = insert(res->argv, word);
	res->Wr = 0;
	res->Rd = Pipeflag;
	return res;
}

tree*add_node(tree*res, char*word)
{
	if(!res)
	{
		res = create_node(word);
		return res;
	}
	tree*tmp = res;
	if(!strcmp(word, ";"))
	{
		//ПЕРЕВЕРНУТЬ ДЕРЕВО
		Specflag = 1;
	}
	else if(!strcmp(word, "||") || !strcmp(word, "&&"))
	{
		//ПЕРЕВЕРНУТЬ ДЕРЕВО
		Specflag = 1;
	}
	else if(!strcmp(word, "|"))
	{
		if(Specflag) return NULL;
		Specflag = 1;
		Pipeflag = 1;
	}
	else	//NEED FLAG OF IF THERE WERE SPECIAL SYMBOL BEFORE, IF SO THEN IT IS COMMAND OTHERWISE IT IS AN ARGUMENT
	{
		while(tmp->right) tmp=tmp->right;
		if(Specflag)
		{
			tmp->Wr = Pipeflag;
			tmp->right = create_node(word);
			Specflag = 0;
			Pipeflag = 0;
		}
		else tmp->argv = insert(tmp->argv, word);
		return res;
	}
	return res;
}

tree*maketree(node*list)
{
	tree*res = NULL;
	while(list)
	{
		res = add_node(res, list->word);
		if(!res) return NULL;
		list = list->next;
	}
	return res;
}

void print_tree(tree*T)
{
	if(T)
	{
		print_tree(T->left);
		printf("Rd: %d ", T->Rd);
		print_list(T->argv);
		printf(" Wr: %d", T->Wr);
		printf("\n");
		print_tree(T->right);
	}
}

void delet_tree(tree*T)
{
	if(T)
	{
		delet_tree(T->left);
		delet_tree(T->right);
		delet_list(T->argv);
		free(T);
	}
}



int main(int argc, char **argv)
{
	char*w = NULL;
	char*col = get_random_colour();
	node*list;
	tree *root;
	while(!eoflag)
	{
		printf("%s> %s", col, RESET);
		Newlineflag = 0;
		list = NULL;
		root = NULL;
		while(!eoflag && !Newlineflag)
		{
			w = readword();
			if(w[0]!=0) list = insert(list, w);
			else free(w);
		}
		if(Qflag)
		{
			fprintf(stderr, "Wrong numbers of quotes!\n");
			Qflag = 0;
			delet(list);
			continue;
		}
		if(!eoflag && list)
		{
			root = maketree(list);
			print_tree(root);
			printf("\n\nrun:\n\n");
//			if(root) do_tree(root);
			do_list(list);
		}
		delet_tree(root);
		delet(list);
	}
	printf("\n%s Bye! %s\n", col, RESET);
	return 0;
}
