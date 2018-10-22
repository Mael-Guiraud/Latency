#include "structs.h"
#include "init.h"
#include "treatment.h"
#include <stdio.h>

void affiche_periode(int * p, int size)
{
	int old, current;
	old = p[0];
	printf("%d[%d-",old,0);
	for(int i=1;i<size;i++)
	{
		current = p[i];
		if(old != current)
		{
			printf("%d]  %d[%d-",i-1,current,i);
		}
		old = current;
	}
	printf("%d]\n",size);
}

void affiche_graph(Graph g,int p)
{
	for(int i=0;i<g.nb_routes; i++)
	{
		printf("Route %d total length = %d : \n",i,route_length( g,i));
		for(int j=0 ; j< g.size_routes[i];j++)
		{
			printf("[%d], partagé avec %d routes\n",g.routes[i][j]->length,g.routes[i][j]->nb_routes);
			if( g.routes[i][j]->period_f != NULL)
			{
				printf(" Forward  \n");
				affiche_periode(g.routes[i][j]->period_f,p);
				printf("Backward \n");
				affiche_periode(g.routes[i][j]->period_b,p);
			}
		}
		printf("\n");
	}
	printf("Fin\n");
	return;
}
void affiche_biparti(int ** g, int a, int b)
{
	for(int i=0;i<a;i++)
	{
		for(int j=0;j<b;j++)
		{
			printf("(%d)",g[i][j]);
		}
		printf("\n");
	}
}
void affiche_assignment(Assignment a,int nb_routes)
{

	for( int i=0;i<nb_routes;i++)
	{
		printf("Route %d : f :%d- b :%d\n",i,a->offset_forward[i],a->offset_backward[i]);
	}
}