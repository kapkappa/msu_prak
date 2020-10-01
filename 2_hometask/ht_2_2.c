#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node list;

struct node{
	int numb;
	list *next;
};

void insert(list* prev, list* lt, int num)
{
	if(lt == NULL)
	{
		list* new = (list*)malloc(sizeof(list));
		new->next = NULL;
		new->numb = num;
		lt = new;
		prev->next=lt;
	}
	else
	{
		if(lt->numb < num) insert(prev->next, lt->next, num);
		else
		{
			list*new = (list*)malloc(sizeof(list));
			new->next = lt;
			new->numb = num;
			prev->next = new;
		}
	}
}

void vivod(list* lt)
{
	printf("Vivod:  ");
	list* tmp = NULL;
	tmp = lt;
	while(tmp!=NULL)
	{
		printf("%d ", tmp->numb);
		tmp = tmp->next;
	}
	printf("\n");
}

void delet(list*lt)
{
	if(lt->next != NULL) delet(lt->next);
	free(lt);
}

int main(int argc, char**argv)
{
	list* head = (list*)malloc(sizeof(list));
	head->next = NULL;
	int c, number = 0, flag = 0, sign = 1;
	FILE* f = NULL;
	if(argc==2) f = fopen(argv[1],"r");
	while ( ( (f != NULL) && ( (c=fgetc(f)) != EOF) ) || ( (c=getchar()) != '\n') )
	{
		if (isdigit(c))
		{
			number =  number * 10 + c - '0';
			flag = 1;
		}
		else
		{
			if(flag)
			{
				if (!sign) number*= -1;
				insert(head, head->next, number);
			}
			flag = 0;
			number = 0;
			sign = c != '-';
		}
	}
	if (flag) insert(head, head->next, number);
	vivod(head->next);
	delet(head);
	if(f!=NULL) fclose(f);
	return 0;
}
