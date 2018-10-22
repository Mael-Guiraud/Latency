#include "structs.h"
#include <stdio.h>
#include <stdlib.h>
//return 1 if the message can reach the destination without collision, 0 otherwise
int message_collisions(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{
	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g.size_routes[route];i++)
		{
			offset += g.routes[route][i]->length; 
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				
				if(g.routes[route][i]->period_f[offset%P] || g.routes[route][i]->period_f[(offset+message_size-1)%P] )
				{
					return 0;
				}
				
			}
		}
	}
	else
	{
		//For each arcs
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			offset += g.routes[route][i]->length; 
			//This is a contention point
			if(g.routes[route][i]->period_b != NULL)
			{
				if(g.routes[route][i]->period_b[offset%P] || g.routes[route][i]->period_b[(offset+message_size-1)%P] )
				{
					return 0;
				}
				
			}
		}
	}
	return 1;

				
}

void fill_period(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{

	//For each arcs
	for(int i=0;i<g.size_routes[route];i++)
	{
		offset += g.routes[route][i]->length;
		//This is a contention point
		if(g.routes[route][i]->period_f != NULL)
		{
			for(int j=0;j<message_size;j++)
			{
				if(kind == FORWARD)
					g.routes[route][i]->period_f[(offset+j)%P]=1;
				else
					g.routes[route][i]->period_b[(offset+j)%P]=1;
			}
				
			
		}
	}
}
int route_length(Graph g,int route)
{
	int length = 0;
	//printf("%d \n",g.size_routes[route]);
	//For each arcs
	for(int i=0;i<g.size_routes[route];i++)
	{
		length += g.routes[route][i]->length;
	}
	return length;
}

void free_assignment(Assignment a)
{
	free(a->offset_forward);
	free(a->offset_backward);
	free(a);
}
void free_graph(Graph g)
{
	for(int i=0;i<g.nb_routes;i++)
	{
		for(int j=0;j<g.size_routes[i];j++)
		{
			if(g.routes[i][j]->nb_routes > 1)
			{
				free(g.routes[i][j]->period_f);
				free(g.routes[i][j]->period_b);
				g.routes[i][j]->nb_routes = 0;
			}
		}
	}
	free(g.arc_pool);
	for(int i=0;i<g.nb_routes;i++)
		free(g.routes[i]);
	free(g.routes);
	free(g.size_routes);
}

//trie le tableau "odre"
//Ordre contient les indices de routes, mais il est trié en fonction des tailles de "tab"
// De la plus longue a la plus courte

void tri_bulles(int* tab,int* ordre,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=tab[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]>tabcpy[j])
			{
				tmp_ordre = ordre[j+1];
				ordre[j+1]=ordre[j];
				ordre[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}