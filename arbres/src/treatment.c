#include "structs.h"
#include "treatment.h"
#include <stdio.h>
#include <stdlib.h>
//return 1 if the message can reach the destination without collision, 0 otherwise
int message_no_collisions(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{
	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g.size_routes[route];i++)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				
				if(g.routes[route][i]->period_f[offset%P] || g.routes[route][i]->period_f[(offset+message_size-1)%P] )
				{
					return 0;
				}
				
			}
			offset += g.routes[route][i]->length; 
		}
	}
	else
	{
		//For each arcs
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_b != NULL)
			{
				if(g.routes[route][i]->period_b[offset%P] || g.routes[route][i]->period_b[(offset+message_size-1)%P] )
				{
					return 0;
				}
				
			}
			offset += g.routes[route][i]->length; 
		}
	}
	return 1;

				
}

void fill_period(Graph g,int route,int offset,int message_size,Period_kind kind,int P)
{

	if(kind == FORWARD)
	{
		//For each arcs
		for(int i=0;i<g.size_routes[route];i++)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					if(route == 0)
						g.routes[route][i]->period_f[(offset+j)%P]=-1;
					else
						g.routes[route][i]->period_f[(offset+j)%P]=route;
				}
					
				
			}
			offset += g.routes[route][i]->length;
		}
	}
	else
	{
		//For each arcs
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{
			
			//This is a contention point
			if(g.routes[route][i]->period_f != NULL)
			{
				for(int j=0;j<message_size;j++)
				{
					if(route ==0)
						g.routes[route][i]->period_b[(offset+j)%P]=-1;
					else
						g.routes[route][i]->period_b[(offset+j)%P]=route;
				}
					
				
			}
			offset += g.routes[route][i]->length;
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
int * routes_by_id(Arc a)
{
	int * id = (int*)malloc(sizeof(int)*a.nb_routes);
	for(int i=0;i<a.nb_routes;i++)
	{
		id[i]=a.routes_id[i];
	}
	return id;
}
int * sort_longest_routes_on_arc(Graph g, Arc a)
{
	int * id = (int*)malloc(sizeof(int)*a.nb_routes);
	int routes_length[a.nb_routes];
	for(int i=0;i<a.nb_routes;i++)
	{
		id[i]=a.routes_id[i];
		routes_length[i]=route_length(g,a.routes_id[i]);
	}
	tri_bulles(routes_length,id,a.nb_routes);
	return id;
}

int longest_route(Graph g)
{
	int max = 0;
	int tmp;
	for(int i=0;i<g.nb_routes;i++)
	{
		tmp = route_length(g,i);
		max = (max>tmp)?max:tmp;
	}
	return max;
}
int route_length_untill_arc(Graph g,int route, Arc * a,Period_kind kind)
{
	int length = 0;
	
	if(kind == FORWARD)
	{
		for(int i=0;i<g.size_routes[route];i++)
		{

			
			if(a == g.routes[route][i])
				return length;
			length += g.routes[route][i]->length;
		}
		
	}
	else
	{
		for(int i=g.size_routes[route]-1;i>=0;i--)
		{

			if(a == g.routes[route][i])
				return length;
			length += g.routes[route][i]->length;
		}
		
	}	
	return length;		
	
}



void free_assignment(Assignment a)
{
	free(a->offset_forward);
	free(a->offset_backward);
	free(a->waiting_time);
	free(a);
}
void free_graph(Graph g)
{
	
	for(int i=0;i<g.nb_routes;i++)
	{
		for(int j=0;j<g.size_routes[i];j++)
		{
			if(g.routes[i][j]->nb_routes >= 1)
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
//Ordre contient les indices des elements, mais il est trié en fonction des valeurs de "tab"
// Du plus grand au plus petit

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


//Pareil mais trié du plus petit au plus grand
void tri_bulles_inverse(int* tab,int* ordre,int taille)
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

			if(tabcpy[j+1]<tabcpy[j])
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


int travel_time_max(Graph g, int tmax, Assignment a)
{

	for(int i=0;i<g.nb_routes;i++)
	{

		if( (2*route_length( g,i) + a->waiting_time[i] ) > tmax )
			return 0;
	}
	return 1;
}

int * load_links(Graph g)
{
	int load[g.arc_pool_size];
	int * id = malloc(sizeof(int)*g.arc_pool_size);
	for(int i=0;i<g.arc_pool_size;i++)
	{
		load[i] = g.arc_pool[i].nb_routes;
		id[i] = i;
	}
	tri_bulles(load,id,g.arc_pool_size);
	return id;
}

int  load_max(Graph g)
{
	int loadmax=0;
	for(int i=0;i<g.arc_pool_size;i++)
	{
	
		if(g.arc_pool[i].nb_routes > loadmax)
		loadmax = g.arc_pool[i].nb_routes;

	}
	
	return loadmax;
}