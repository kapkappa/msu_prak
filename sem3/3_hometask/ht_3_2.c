#include <stdio.h>
#include <stdlib.h>

typedef struct tree Tree;

struct tree{
	int number;
	Tree *left;
	Tree *right;
};

#define FINDKEY 10
#define DELKEY 3

Tree * add_node(Tree*node, int num)
{
	if(node == NULL)
	{
		Tree*tmp = (Tree*)malloc(sizeof(Tree));
		tmp->number = num;
		tmp->left = tmp->right = NULL;
		return tmp;
	}
	else if(node->number < num)
		node->right = add_node(node->right, num);
	else if(node->number > num)
		node->left = add_node(node->left, num);
	return node;
}

Tree * get_left(Tree*node)
{
	if(node->left == NULL)
		return node;
	else
		node = get_left(node->left);
	return node;
}

Tree * delete_node(Tree*node, int num)
{
	if(node==NULL)
	{
		printf("There is no such node\n");
		return NULL;
	}
	else if(node->number < num)
	{
		node->right = delete_node(node->right, num);
	}
	else if(node->number > num)
	{
		node->left = delete_node(node->left, num);
	}
	else // delete this node, check for children and parents.
	if(node->left == NULL && node->right == NULL)
	{
		free(node);
		return NULL;
	}
	else if(node->left == NULL || node->right == NULL)
	{
		Tree*tmp=NULL;
		if(node->left == NULL)
			tmp = node->right;
		else if(node->right == NULL)
			tmp = node->left;
		free(node);
		return tmp;
	}
	else if(node->right->left==NULL)
	{
		node->right->left = node->left;
		Tree*tmp = node->right;
		free(node);
		return tmp;
	}
	else
	{
		Tree*tmp = get_left(node->right);
		int t = tmp->number;
		delete_node(node, t);
		node->number = t;
		return node;
	}
	return node;
}

Tree * find_node(Tree*node, int num)
{
	if(node == NULL || node->number == num)
	{
		return node;
	}
	else if(node->number < num)
	{
		node = find_node(node->right, num);
	}
	else
		node = find_node(node->left, num);
	return node;
}


void printT(Tree*node)
{
	if(node!=NULL)
	{
		printT(node->left);
		printf("%d ", node->number);
		printT(node->right);
	}
}

void delete_T(Tree *node)
{
	if(node!=NULL)
	{
		delete_T(node->left);
		delete_T(node->right);
		free(node);
	}
}

int main()
{
	Tree *root = NULL;
	char c=0;
	int num;
	while((c=fgetc(stdin))!=EOF)
	{
		ungetc(c, stdin);
		scanf("%d",&num);
		root = add_node(root, num);
	}
	printT(root);
	printf("\n");

	Tree *node = find_node(root, FINDKEY);
	if(node==NULL)
		printf("There is no such node.\n");
	else
		printf("In Node: %d\n", node->number);

	delete_node(root, DELKEY);
	printT(root);
	printf("\n");

	delete_T(root);
	return 0;
}
