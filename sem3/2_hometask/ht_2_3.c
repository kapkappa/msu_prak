#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG
void vivod(int*mas, int i)
{
	int j;
	for(j=0;j<i;j++) printf("%d ", mas[j]);
	printf("\n");
}
#endif

void primes(int n) //РЕШЕТО ЭРАТОСФЕНА
{
	n-=1;
	int * mas = (int*)malloc(n*sizeof(int));
	int i, j;
	for(i=0;i<n;i++) mas[i]=i+2;

	double num = sqrt(n);
	double supremum = ceil(num);
	int S = (int)supremum;

	for(i=0;i<S-1;i++)
	{
		if(mas[i]!=0)
		{
			for(j=i+1;j<n;j++)
			{
				if(mas[j] % mas[i] == 0) mas[j]=0;
			}
		}
	}

	for(i=0;i<n;i++)
	{
		if(mas[i]!=0) printf("%d ", mas[i]);
	}
	free(mas);
	printf("\n");
}

void fibprimes(int n)
{
	int SIZE = 30;
	long int f1 = 2, f2 = 3;
	long int *fmas = (long int *)malloc(SIZE*sizeof(long int));
	fmas[0] = f1;
	fmas[1] = f2;
	long int sum = f2;
	int i=2;
	do
	{
		if(SIZE==i){ SIZE*=2; fmas = realloc(fmas, SIZE*sizeof(long int));}
		fmas[i] = sum + fmas[i-2];
		sum = fmas[i];
		i++;
	}while(sum < n);
	int l,j;
	int num = (int)ceil(sqrt(n));
	for(l=2;l<=num;l++)
	{
		for(j=0;j<i;j++)
		{
			if((fmas[j] != l) && (fmas[j] % l == 0)) fmas[j]=0;
		}
	}
	for(j=0;j<i;j++)
	{
		if(fmas[j] != 0) printf("%ld ", fmas[j]);
	}
	free(fmas);
	printf("\n");
}


int main()
{
	int n;
	scanf("%d", &n);
	printf("Prime numbers: \n");
	primes(n);
	printf("Fibonachi prime numbers: \n");
	fibprimes(n);
}
