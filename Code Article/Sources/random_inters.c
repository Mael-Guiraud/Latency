
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define NB_COMP 4

typedef struct { uint64_t number[NB_COMP]; } lint;


lint add(lint a, lint b)
{
	int carry = 0;
	lint res;
	for(int i=0;i<NB_COMP;i++)
	{
		if(carry)
		{
			if(a.number[i] != UINT64_MAX)
				a.number[i]++;
			else
				if(b.number[i] != UINT64_MAX)
					b.number[i]++;
				else
				{
					res.number[i] = UINT64_MAX;
					carry = 1;
					continue;
				}
		}
		res.number[i] = UINT64_MAX - a.number[i];

		if(res.number[i] < b.number[i] )
			carry = 1;
		else
			carry = 0;

		res.number[i]= a.number[i]+b.number[i];
	}

	return res;
}

lint random_lint()
{
	lint a;
	for(int i=0;i<NB_COMP;i++)
		a.number[i] = rand();
	return a;
}

lint init_lint()
{
	lint a;
	for(int i=0;i<NB_COMP;i++)
		a.number[i] = 0;
	return a;
}

void print_lint(lint a)
{
	for(int i=NB_COMP-1;i>=0;i--)
	{
		printf("[%llu]", a.number[i]);
	}
	printf(" ");
}

lint ** init_lint_table(int n, int k)
{
	lint ** table = (lint**)malloc(sizeof(lint*)*k);
	for(int i=0;i<k;i++)
	{	
		table[i] = (lint*)malloc(sizeof(lint)*n);
		for(int j=0;j<n;j++)
		{
			table[i][j] = init_lint();
		}
		table[i][0].number[0] = 1;
	}
	for(int j=0;j<n;j++)
	{
		table[0][j].number[0] = 1;
	}
	return table;
}

lint ** pnk(lint ** table,int n,int k)
{
	for(int i=1;i<k;i++)
	{
		for(int j=1;j<n;j++)
		{
			table[i][j] = add(table[i-1][j],table[i][j-1]);
		}
	}
	return table;
}

lint ** compute_pkn(int n, int k)
{
	lint ** table = init_lint_table(n,k);
	table = pnk(table,n,k);

	return table;
}

void print_table(lint ** table, int n, int k)
{
	for(int i=0;i<k;i++)
	{
		printf("k = %d : ",i);
		for(int j=0;j<n;j++)
		{
			print_lint(table[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
//return 1 if a>b, -1 if b>a and 0 if a=B
int cmp(lint a, lint b)
{
	for(int i=NB_COMP-1;i>=0;i--)
	{
		if(a.number[i] > b.number[i]) return 1;
		if(a.number[i] < b.number[i]) return -1;
	}
	return 0;
}


lint random_lint_max(lint max)
{
	int i;
	lint lrandom = init_lint();
	for(i=NB_COMP-1;i>=0 && !max.number[i]; i--)
	if (i < 0) return lrandom;	
	lrandom.number[i] = ((random() << 32) | random()) % (max.number[i] + 1);
	i--;
	for(;i>=0; i--) lrandom.number[i] =  (random() << 32) | random(); //compute the random number
	if(cmp(lrandom,max) >= 0) return random_lint_max(max);
	//reject if not legal (with the right probability to respect uniformity)
	return lrandom;
}
/*
lint random_lint_max(lint max)
{
	uint64_t x;
	lint new;
	int i;
	int begin;
	int critical= 1;
	for(i=NB_COMP-1;i>=0;i--)
	{
		if(!max.number[i])
			new.number[i]=0;
		else 
			break;
	}
	begin = i;
	if(i==0)
	{
		new.number[i] = ( (random() << 32) | random()) % max.number[i];
	}


	while(i >= 0)
	{
		if(i == begin)
		{
			x = ( (random() << 32) | random()) % max.number[i];
			critical = (x==max.number[i]);
		}
		else
		{
			x = ( (random() << 32) | random());
			if(critical)
			{
				if(x > max.number[i])
				{
					i++;
					continue;
				}
			}
			critical = (x == max.number[i]);
		}
		new.number[i] = x;
		i--;
	}
	
	return new;
	
}*/

int transfo(lint a, lint ** table, int n, int k)
{
	int i=0;

	for(i=0; cmp(a,table[k][i]) != -1;i++);
	return n-i;

}
void free_table(lint ** table, int k)
{
	for(int i=0;i<k;i++)
		free(table[i]);
	free(table);
}
int * random_inters(int n, int k)
{
	lint ** table = compute_pkn(n+1,k);
	int * result = (int * )malloc(sizeof(int)*k);
	int X;
	lint new;
	int size_tab = k;
	lint a;
	for(int i=0;i<size_tab-1;i++)
	{	
		a = table[k-1][n];
		//print_lint(a);printf("a \n");
		new = random_lint_max(a);
		//print_lint(new);printf("new \n");
		X = transfo(new,table,n,k-1);
		result[i] = X;
		//printf("%d \n",X);
		n-=X;
		//printf("%d n\n",n);

		k--;
		//print_lint(table[k-1][n]);printf("p \n");
	}
	result[size_tab-1] = n;
	free_table(table, size_tab);
	return result;
}







