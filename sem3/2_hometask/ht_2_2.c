#include <stdlib.h>
#include <stdio.h>

#define N 10

void remap(int a[N])
{
	int i;
	int FLAG = 1;
	for(i=0;i<N;i++)
	{
		if(a[i]>0)
		{
			int k;
			for(k=i;k<N && a[k]>0;k++);
			if(k<N)
			{
				int tmp = a[k];
				a[k] = a[i];
				a[i] = tmp;
			}else FLAG=0;
		}
		if(!FLAG) break;
	}
	int j;
	for(j=0;j<i;j++)
	{
		if(a[j]==0)
		{
			int k;
			for(k=j;k<i && a[k] == 0;k++);
			if(k<i)
			{
				int tmp = a[k];
				a[k] = a[j];
				a[j] = tmp;
			}else FLAG = -1;
		}
		if(FLAG == -1) j=i;
	}
}

int main()
{
	int *mas = (int*)malloc(N * sizeof(int));
	int i;
	for(i=0;i<N;i++)
	{
		scanf("%d", mas+i);
	}
	printf("\n");
	remap(mas);
	for(i=0;i<N;i++)
	{
		printf("%d  ", mas[i]);
	}
	printf("\n");
	free(mas);
}
