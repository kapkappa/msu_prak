#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define INERR "Error, cant open the input file!\n"
#define OUTERR "Error, cant open the output file!\n"

typedef struct tree T;

struct tree {
	char* word;
	int count;
	struct tree *left;
	struct tree *right;
};

char* getword(FILE *f)
{
	int size = 4, i=0;
	char *word = (char*)malloc(size * sizeof(char));
	char c;
	while(isspace(c=fgetc(f)) || iscntrl(c)); //Skipping Delimiters
	while(isalnum(c))//Пока нужный нам символ - записываем.
	{
		word[i++] = c;
		if((i+1) == size)
		{
			size*=2;
			word = realloc(word, size*sizeof(char));
		}
		c=fgetc(f);
	}
	if(i==0 && c!=EOF) //C not a Delimiter and not a common symbol and not EOF -> c is special symbol.
	{
		word[i++]=c;
	}
	else ungetc(c, f);
	word[i] = 0;
	return word;
}

T* add_word(T *Tree, char *word)
{
	if(Tree==NULL) // Including our word in tree.
	{
		Tree = (T*)malloc(sizeof(T));
		Tree->word = word;
		Tree->count = 1;
		Tree->left = NULL;
		Tree->right = NULL;
	}
	else
	{
		int cmp = strcmp(word, Tree->word);
		if(!cmp)
		{
			free(word);
			Tree->count+=1;
		}
		else if(cmp < 0) Tree->left = add_word(Tree->left, word);
		else if(cmp > 0) Tree->right = add_word(Tree->right, word);
	}
	return Tree;
}

void vivod(T *Tree, FILE *f, int cnt)
{
	if(Tree!=NULL)
	{
		vivod(Tree->left, f, cnt);
		double all = (double)Tree->count / (double)cnt;
		fprintf(f, "%s %d %f\n", Tree->word, Tree->count, all);
		vivod(Tree->right, f, cnt);
	}
}

void delet(T *Tree)
{
	if(Tree!=NULL)
	{
		delet(Tree->left);
		delet(Tree->right);
		free(Tree->word);
		free(Tree->left);
		free(Tree->right);
	}
}

int main(int argc, char**argv)
{
	FILE *fin=stdin, *fout=stdout;
	if(argc>=3)
	{
		if(!strcmp(argv[1],"-i"))
		{
			fin = fopen(argv[2],"r");
			if(fin==NULL){fprintf(stderr, INERR); exit(1);}
		}
		else if(!strcmp(argv[1], "-o"))
		{
			fout = fopen(argv[2], "w");
			if(fout==NULL){fprintf(stderr, OUTERR); exit(1);}
		}
	}
	if(argc>=5)
	{
		if(!strcmp(argv[3], "-o"))
		{
			fout = fopen(argv[4], "w");
			if(fout==NULL){fprintf(stderr, OUTERR); exit(1);}
		}
	}
	int cnt = 0;
	char *word = NULL;
	T *Head_tree = NULL;
	while(!feof(fin))
	{
		word = getword(fin);
		Head_tree = add_word(Head_tree, word);
		cnt++;
	}
	printf("%d\n",cnt);
	vivod(Head_tree, fout, cnt);
	delet(Head_tree);
	free(Head_tree);
	fclose(fin);
	fclose(fout);
	return 0;
}
